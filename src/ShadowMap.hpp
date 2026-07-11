#ifndef COSMOS_SHADOWMAP_HPP_
#define COSMOS_SHADOWMAP_HPP_

#include <glm/glm.hpp>

class ShadowMap {
public:
    ShadowMap();

    void update(const glm::vec3& light_direction, const glm::mat4& cascade_proj_view);

    glm::mat4 view_matrix() const;
    const glm::mat4& projection() const { return projection_; }
    glm::mat4 tex_matrix() const;
    float depth_range() const { return depth_range_; }

private:
    glm::vec3 position_;
    glm::vec3 direction_;
    glm::mat4 projection_;
    float depth_range_;

    inline static const glm::mat4 bias_matrix_{
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f};
};

#endif
