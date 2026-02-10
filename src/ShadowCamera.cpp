#include <glm/gtc/matrix_transform.hpp>

#include "ShadowCamera.hpp"
#include "Camera.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "Vector3f.hpp"

GLuint ShadowCamera::frustum_vao_ = 0;
GLuint ShadowCamera::frustum_vbo_ = 0;
bool ShadowCamera::frustum_initialized_ = false;

ShadowCamera::ShadowCamera()
    : position_(5.0f, 15.0f, 5.0f),
      direction_(0.0f, -15.0f, -30.0f),
      projection_(glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 1.0f, 40.0f)),
      enabled_(true) {
}

glm::mat4 ShadowCamera::view_matrix() const {
    return Camera::matrixFromPositionDirection(
        Vector3f(position_.x, position_.y, position_.z),
        Vector3f(direction_.x, direction_.y, direction_.z));
}

glm::mat4 ShadowCamera::tex_matrix() const {
    return bias_matrix_ * projection_ * view_matrix();
}

void ShadowCamera::initFrustumGeometry() {
    // 12 edges of a cube from -1 to 1 as GL_LINES (24 vertices)
    static const float verts[] = {
        // bottom face edges
        -1, -1, -1,   1, -1, -1,
         1, -1, -1,   1, -1,  1,
         1, -1,  1,  -1, -1,  1,
        -1, -1,  1,  -1, -1, -1,
        // top face edges
        -1,  1, -1,   1,  1, -1,
         1,  1, -1,   1,  1,  1,
         1,  1,  1,  -1,  1,  1,
        -1,  1,  1,  -1,  1, -1,
        // vertical edges
        -1, -1, -1,  -1,  1, -1,
         1, -1, -1,   1,  1, -1,
         1, -1,  1,   1,  1,  1,
        -1, -1,  1,  -1,  1,  1,
    };

    glGenVertexArrays(1, &frustum_vao_);
    glGenBuffers(1, &frustum_vbo_);
    glBindVertexArray(frustum_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, frustum_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

    frustum_initialized_ = true;
}

void ShadowCamera::render_frustum(const glm::mat4& mainProjView) const {
    if (!frustum_initialized_) {
        initFrustumGeometry();
    }

    auto flatProg = ShaderManager::get().get_shader_program("flat");
    flatProg->run();

    // NDC cube -> world: inverse of (projection * view) maps [-1,1]^3 to the shadow frustum in world space
    // Then mainProjView projects that into screen space
    glm::mat4 mvp = mainProjView * glm::inverse(projection_ * view_matrix());
    flatProg->setMat4("mvp", mvp);

    GLint loc = glGetUniformLocation(flatProg->get_id(), "flatColor");
    if (loc >= 0) glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    // Save GL state
    GLint prevPolygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, prevPolygonMode);
    GLboolean prevCullFace = glIsEnabled(GL_CULL_FACE);
    GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(frustum_vao_);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);

    // Restore GL state
    glPolygonMode(GL_FRONT_AND_BACK, prevPolygonMode[0]);
    if (prevCullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
}
