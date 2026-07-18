#ifndef COSMOS_SHADOWMAPMANAGER_HPP_
#define COSMOS_SHADOWMAPMANAGER_HPP_

#include <array>
#include <functional>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "ShadowMap.hpp"

class ShadowMapManager {
public:
    static constexpr int MAX_CASCADES = 4;

    ShadowMapManager();
    ~ShadowMapManager();

    void init(int map_size);

    void update(const glm::vec3& light_direction, const glm::mat4& camera_view,
                float fov_radians, float aspect, float near_plane,
                int cascade_count, float shadow_distance);

    void render(const std::function<void(const ShadowMap&, int)>& draw_callback);

    void bind_shadow_texture(GLenum unit) const;

    glm::mat4 shadow_matrix(int cascade) const;
    glm::vec4 cascade_splits_eye() const;
    glm::vec4 cascade_biases(float base_bias) const;
    int cascade_count() const { return cascade_count_; }

    void render_debug_frustums(const glm::mat4& main_proj_view) const;

private:
    std::array<ShadowMap, MAX_CASCADES> cascades_;
    float splits_[MAX_CASCADES + 1];
    int cascade_count_;
    int map_size_;
    GLuint fbo_;
    GLuint depth_array_tex_;

    static GLuint frustum_vao_;
    static GLuint frustum_vbo_;
    static bool frustum_initialized_;
    static void init_frustum_geometry();

    ShadowMapManager(const ShadowMapManager&) = delete;
    ShadowMapManager& operator=(const ShadowMapManager&) = delete;
};

#endif
