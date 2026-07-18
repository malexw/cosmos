#ifndef COSMOS_RENDER_PASS_HPP_
#define COSMOS_RENDER_PASS_HPP_

#include <glad/gl.h>

#include <functional>

#include "RenderTarget.hpp"

struct RenderPassParams {
    RenderTarget* target = nullptr;  // nullptr = default framebuffer
    int viewport_width  = 0;         // 0 = use target dimensions
    int viewport_height = 0;

    bool clear_color = false;
    bool clear_depth = false;

    bool depth_test  = true;
    bool depth_write = true;
    bool color_write = true;
    GLenum cull_face = GL_BACK;
};

class RenderPass {
public:
    static void execute(const RenderPassParams& params,
                        const std::function<void()>& draw);
};

#endif
