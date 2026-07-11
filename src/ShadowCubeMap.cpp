#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "ShadowCubeMap.hpp"

ShadowCubeMap::ShadowCubeMap()
    : position_(0.0f),
      near_(0.1f),
      far_(10.0f),
      map_size_(1024),
      fbo_(0),
      depth_cube_tex_(0) {
}

ShadowCubeMap::~ShadowCubeMap() {
    if (depth_cube_tex_) glDeleteTextures(1, &depth_cube_tex_);
    if (fbo_) glDeleteFramebuffers(1, &fbo_);
}

void ShadowCubeMap::init(int map_size) {
    map_size_ = map_size;

    glGenTextures(1, &depth_cube_tex_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_tex_);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
                     map_size_, map_size_, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Faces are attached one at a time during render
    glGenFramebuffers(1, &fbo_);
}

void ShadowCubeMap::update(const glm::vec3& position, float radius) {
    position_ = position;
    far_ = std::max(radius, near_ + 0.1f);

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, near_, far_);

    // Standard OpenGL cube map face orientations
    static const glm::vec3 targets[6] = {
        { 1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f},
        { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f, -1.0f},
    };
    static const glm::vec3 ups[6] = {
        {0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f},
        {0.0f,  0.0f,  1.0f}, {0.0f,  0.0f, -1.0f},
        {0.0f, -1.0f,  0.0f}, {0.0f, -1.0f,  0.0f},
    };

    for (int i = 0; i < 6; ++i) {
        glm::mat4 view = glm::lookAt(position_, position_ + targets[i], ups[i]);
        face_matrices_[i] = proj * view;
    }
}

void ShadowCubeMap::render(const std::function<void(const glm::mat4&, int)>& draw_callback) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, map_size_, map_size_);

    // Shadow pass GL state
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glCullFace(GL_FRONT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    for (int i = 0; i < 6; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depth_cube_tex_, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        draw_callback(face_matrices_[i], i);
    }

    // Restore state
    glCullFace(GL_BACK);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowCubeMap::bind_texture(GLenum unit) const {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_tex_);
}
