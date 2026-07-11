#include <algorithm>
#include <cfloat>

#include <glm/gtc/matrix_transform.hpp>

#include "ShadowMap.hpp"
#include "Camera.hpp"
#include "Vector3f.hpp"

ShadowMap::ShadowMap()
    : position_(0.0f),
      direction_(0.0f, -1.0f, 0.0f),
      projection_(1.0f),
      depth_range_(1.0f) {
}

void ShadowMap::update(const glm::vec3& light_direction, const glm::mat4& cascade_proj_view) {
    direction_ = light_direction;

    // 1. Extract 8 frustum corners in world space
    glm::mat4 inv = glm::inverse(cascade_proj_view);
    glm::vec3 corners[8];
    for (int i = 0; i < 8; ++i) {
        glm::vec4 ndc(
            (i & 1) * 2.0f - 1.0f,
            ((i >> 1) & 1) * 2.0f - 1.0f,
            ((i >> 2) & 1) * 2.0f - 1.0f,
            1.0f);
        glm::vec4 world = inv * ndc;
        corners[i] = glm::vec3(world) / world.w;
    }

    // 2. Frustum center
    glm::vec3 center(0.0f);
    for (int i = 0; i < 8; ++i) center += corners[i];
    center /= 8.0f;

    // 3. Compute radius (max distance from center to any corner)
    float radius = 0.0f;
    for (int i = 0; i < 8; ++i)
        radius = std::max(radius, glm::length(corners[i] - center));

    // 4. Position light behind the frustum
    glm::vec3 dir = glm::normalize(direction_);
    position_ = center - dir * radius;

    // 5. Transform corners to light space, compute AABB
    glm::mat4 lv = view_matrix();
    glm::vec3 ls_min(FLT_MAX), ls_max(-FLT_MAX);
    for (int i = 0; i < 8; ++i) {
        glm::vec3 ls = glm::vec3(lv * glm::vec4(corners[i], 1.0f));
        ls_min = glm::min(ls_min, ls);
        ls_max = glm::max(ls_max, ls);
    }

    // 6. Extend near plane to catch shadow casters behind camera
    float depth = ls_max.z - ls_min.z;
    ls_min.z -= depth;

    // 7. Store depth range (used for per-cascade bias scaling)
    depth_range_ = -ls_min.z - (-ls_max.z);  // far - near in ortho terms

    // 8. Set ortho projection
    projection_ = glm::ortho(ls_min.x, ls_max.x, ls_min.y, ls_max.y, -ls_max.z, -ls_min.z);
}

glm::mat4 ShadowMap::view_matrix() const {
    return Camera::matrixFromPositionDirection(
        Vector3f(position_.x, position_.y, position_.z),
        Vector3f(direction_.x, direction_.y, direction_.z));
}

glm::mat4 ShadowMap::tex_matrix() const {
    return bias_matrix_ * projection_ * view_matrix();
}
