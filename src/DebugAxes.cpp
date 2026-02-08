#include "DebugAxes.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager/ShaderManager.hpp"

DebugAxes::DebugAxes() : vao_(0), vbo_(0), initialized_(false) {
}

DebugAxes::~DebugAxes() {
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

void DebugAxes::init() {
    if (initialized_) return;

    // 6 vertices for 3 axis lines (2 per line)
    // Each line goes from origin to unit length along that axis
    float vertices[] = {
        // X axis (red)
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        // Y axis (green)
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // Z axis (blue)
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position at location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    initialized_ = true;
}

void DebugAxes::render(const glm::mat4& model, const glm::mat4& viewProj, float scale) {
    if (!initialized_) return;

    auto flatProg = ShaderManager::get().get_shader_program("flat");
    if (!flatProg) return;

    flatProg->run();

    // Scale the model matrix to adjust axis length
    glm::mat4 scaledModel = model * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    flatProg->setMat4("mvp", viewProj * scaledModel);

    GLint colorLoc = glGetUniformLocation(flatProg->get_id(), "flatColor");

    // Disable depth test so axes render on top of geometry
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(vao_);

    // Draw X axis (red)
    if (colorLoc >= 0) glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 0, 2);

    // Draw Y axis (green)
    if (colorLoc >= 0) glUniform4f(colorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 2, 2);

    // Draw Z axis (blue)
    if (colorLoc >= 0) glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    glBindVertexArray(0);

    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);
}
