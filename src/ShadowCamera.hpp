#ifndef COSMOS_SHADOWCAMERA_HPP_
#define COSMOS_SHADOWCAMERA_HPP_

#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>

#include "util.hpp"

class ShadowCamera {
public:
    ShadowCamera();

    void set_position(const glm::vec3& pos) { position_ = pos; }
    void set_direction(const glm::vec3& dir) { direction_ = dir; }
    void set_projection(const glm::mat4& proj) { projection_ = proj; }
    void set_enabled(bool enabled) { enabled_ = enabled; }

    const glm::vec3& position() const { return position_; }
    const glm::vec3& direction() const { return direction_; }
    const glm::mat4& projection() const { return projection_; }
    bool enabled() const { return enabled_; }

    glm::mat4 view_matrix() const;
    glm::mat4 tex_matrix() const;

    void render_frustum(const glm::mat4& mainProjView) const;

private:
    glm::vec3 position_;
    glm::vec3 direction_;
    glm::mat4 projection_;
    bool enabled_;

    static const glm::mat4 bias_matrix_;

    static GLuint frustum_vao_;
    static GLuint frustum_vbo_;
    static bool frustum_initialized_;
    static void initFrustumGeometry();

    DISALLOW_COPY_AND_ASSIGN(ShadowCamera);
};

#endif
