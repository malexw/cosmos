#ifndef COSMOS_DEBUGAXES_HPP_
#define COSMOS_DEBUGAXES_HPP_

#include <memory>

#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>

class DebugAxes {
public:
    using ShPtr = std::shared_ptr<DebugAxes>;

    DebugAxes();
    ~DebugAxes();

    void init();
    void render(const glm::mat4& model, const glm::mat4& viewProj, float scale = 1.0f);

private:
    GLuint vao_;
    GLuint vbo_;
    bool initialized_;
};

#endif
