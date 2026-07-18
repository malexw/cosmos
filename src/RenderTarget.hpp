#ifndef COSMOS_RENDER_TARGET_HPP_
#define COSMOS_RENDER_TARGET_HPP_

#include <glad/gl.h>

#include <initializer_list>
#include <vector>

struct RenderTargetAttachment {
    GLenum attachment_point;  // GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT
    GLenum internal_format;   // GL_RGBA16F, GL_R8, GL_DEPTH_COMPONENT24
    GLenum filter;            // GL_LINEAR, GL_NEAREST
    GLenum wrap = GL_CLAMP_TO_EDGE;
};

class RenderTarget {
public:
    RenderTarget(int width, int height, std::initializer_list<RenderTargetAttachment> attachments);
    ~RenderTarget();

    RenderTarget(RenderTarget&& other) noexcept;
    RenderTarget& operator=(RenderTarget&& other) noexcept;

    void bind() const;
    static void bind_default();

    int width() const { return width_; }
    int height() const { return height_; }
    GLuint texture(GLenum attachment_point) const;
    GLuint color_texture() const { return texture(GL_COLOR_ATTACHMENT0); }
    GLuint depth_texture() const { return texture(GL_DEPTH_ATTACHMENT); }

private:
    GLuint fbo_ = 0;
    int width_ = 0;
    int height_ = 0;

    struct AttachmentData {
        GLenum point;
        GLuint texture;
    };
    std::vector<AttachmentData> attachments_;

    void destroy();

    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;
};

#endif
