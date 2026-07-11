#include "RenderPass.hpp"

void RenderPass::execute(const RenderPassParams& params,
                         const std::function<void()>& draw) {
    // Bind target
    if (params.target) {
        params.target->bind();
    } else {
        RenderTarget::bind_default();
    }

    // Viewport
    int vp_w = params.viewport_width;
    int vp_h = params.viewport_height;
    if (vp_w == 0 && vp_h == 0 && params.target) {
        vp_w = params.target->width();
        vp_h = params.target->height();
    }
    if (vp_w > 0 && vp_h > 0) {
        glViewport(0, 0, vp_w, vp_h);
    }

    // GL state
    glColorMask(params.color_write, params.color_write,
                params.color_write, params.color_write);
    glCullFace(params.cull_face);
    glDepthMask(params.depth_write ? GL_TRUE : GL_FALSE);

    if (params.depth_test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    // Clear
    GLbitfield clear_bits = 0;
    if (params.clear_color) clear_bits |= GL_COLOR_BUFFER_BIT;
    if (params.clear_depth) clear_bits |= GL_DEPTH_BUFFER_BIT;
    if (clear_bits) {
        glClear(clear_bits);
    }

    // Draw
    draw();
}
