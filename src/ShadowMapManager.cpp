#include <cfloat>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "ShadowMapManager.hpp"
#include "ResourceManager/ShaderManager.hpp"

GLuint ShadowMapManager::frustum_vao_ = 0;
GLuint ShadowMapManager::frustum_vbo_ = 0;
bool ShadowMapManager::frustum_initialized_ = false;

ShadowMapManager::ShadowMapManager()
    : splits_{},
      cascade_count_(1),
      map_size_(2048),
      fbo_(0),
      depth_array_tex_(0) {
}

ShadowMapManager::~ShadowMapManager() {
    if (depth_array_tex_) glDeleteTextures(1, &depth_array_tex_);
    if (fbo_) glDeleteFramebuffers(1, &fbo_);
}

void ShadowMapManager::init(int map_size) {
    map_size_ = map_size;

    // Create depth array texture (one layer per cascade)
    glGenTextures(1, &depth_array_tex_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_array_tex_);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24,
                 map_size_, map_size_, MAX_CASCADES, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create FBO (layers will be attached per-cascade during render)
    glGenFramebuffers(1, &fbo_);
}

void ShadowMapManager::update(const glm::vec3& light_direction, const glm::mat4& camera_view,
                               float fov_radians, float aspect, float near_plane,
                               int cascade_count, float shadow_distance) {
    cascade_count_ = std::min(cascade_count, MAX_CASCADES);
    float near = near_plane;
    float far = shadow_distance;

    // Practical split scheme (lambda=0.5 blend of log and uniform)
    splits_[0] = near;
    for (int i = 1; i < cascade_count_; ++i) {
        float p = static_cast<float>(i) / static_cast<float>(cascade_count_);
        float log_split = near * std::pow(far / near, p);
        float uni_split = near + (far - near) * p;
        splits_[i] = 0.5f * log_split + 0.5f * uni_split;
    }
    splits_[cascade_count_] = far;

    // Update each cascade
    for (int i = 0; i < cascade_count_; ++i) {
        glm::mat4 sub_proj = glm::perspective(fov_radians, aspect, splits_[i], splits_[i + 1]);
        glm::mat4 cascade_proj_view = sub_proj * camera_view;
        cascades_[i].update(light_direction, cascade_proj_view);
    }
}

void ShadowMapManager::render(const std::function<void(const ShadowMap&, int)>& draw_callback) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, map_size_, map_size_);

    // Shadow pass GL state
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glCullFace(GL_FRONT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    for (int i = 0; i < cascade_count_; ++i) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  depth_array_tex_, 0, i);
        glClear(GL_DEPTH_BUFFER_BIT);
        draw_callback(cascades_[i], i);
    }

    // Restore state
    glCullFace(GL_BACK);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapManager::bind_shadow_texture(GLenum unit) const {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_array_tex_);
}

glm::mat4 ShadowMapManager::shadow_matrix(int cascade) const {
    return cascades_[cascade].tex_matrix();
}

glm::vec4 ShadowMapManager::cascade_biases(float base_bias) const {
    glm::vec4 result(0.0f);
    for (int i = 0; i < cascade_count_; ++i) {
        float dr = cascades_[i].depth_range();
        result[i] = (dr > 0.0f) ? base_bias / dr : 0.001f;
    }
    return result;
}

glm::vec4 ShadowMapManager::cascade_splits_eye() const {
    glm::vec4 result(FLT_MAX);
    for (int i = 0; i < cascade_count_; ++i) {
        result[i] = splits_[i + 1];
    }
    return result;
}

void ShadowMapManager::init_frustum_geometry() {
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

void ShadowMapManager::render_debug_frustums(const glm::mat4& main_proj_view) const {
    if (!frustum_initialized_) {
        init_frustum_geometry();
    }

    auto flatProg = ShaderManager::get().get_shader_program("flat");
    flatProg->run();

    // Save GL state
    GLint prevPolygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, prevPolygonMode);
    GLboolean prevCullFace = glIsEnabled(GL_CULL_FACE);
    GLboolean prevDepthTest = glIsEnabled(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    static const glm::vec4 cascade_colors[] = {
        {1.0f, 0.0f, 0.0f, 1.0f},  // red
        {0.0f, 1.0f, 0.0f, 1.0f},  // green
        {0.0f, 0.0f, 1.0f, 1.0f},  // blue
        {1.0f, 1.0f, 0.0f, 1.0f},  // yellow
    };

    for (int i = 0; i < cascade_count_; ++i) {
        glm::mat4 mvp = main_proj_view * glm::inverse(cascades_[i].projection() * cascades_[i].view_matrix());
        flatProg->setMat4("mvp", mvp);

        GLint loc = glGetUniformLocation(flatProg->get_id(), "flatColor");
        if (loc >= 0) {
            const glm::vec4& c = cascade_colors[i];
            glUniform4f(loc, c.r, c.g, c.b, c.a);
        }

        glBindVertexArray(frustum_vao_);
        glDrawArrays(GL_LINES, 0, 24);
    }
    glBindVertexArray(0);

    // Restore GL state
    glPolygonMode(GL_FRONT_AND_BACK, prevPolygonMode[0]);
    if (prevCullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (prevDepthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
}
