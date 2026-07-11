#include "RenderTarget.hpp"

#include <algorithm>

namespace {

bool is_depth_format(GLenum format) {
    return format == GL_DEPTH_COMPONENT16 ||
           format == GL_DEPTH_COMPONENT24 ||
           format == GL_DEPTH_COMPONENT32F;
}

GLenum pixel_format_for(GLenum internal_format) {
    if (is_depth_format(internal_format)) return GL_DEPTH_COMPONENT;
    if (internal_format == GL_R8) return GL_RED;
    return GL_RGBA;
}

GLenum pixel_type_for(GLenum internal_format) {
    if (internal_format == GL_RGBA16F || internal_format == GL_DEPTH_COMPONENT32F)
        return GL_FLOAT;
    return GL_UNSIGNED_BYTE;
}

} // namespace

RenderTarget::RenderTarget(int width, int height,
                           std::initializer_list<RenderTargetAttachment> attachments)
    : width_(width), height_(height) {

    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    bool has_color = false;

    for (const auto& att : attachments) {
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, att.internal_format, width, height, 0,
                     pixel_format_for(att.internal_format),
                     pixel_type_for(att.internal_format), nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, att.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, att.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, att.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, att.wrap);
        glFramebufferTexture2D(GL_FRAMEBUFFER, att.attachment_point, GL_TEXTURE_2D, tex, 0);

        if (!is_depth_format(att.internal_format)) {
            has_color = true;
        }

        attachments_.push_back({att.attachment_point, tex});
    }

    if (!has_color) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
    destroy();
}

RenderTarget::RenderTarget(RenderTarget&& other) noexcept
    : fbo_(other.fbo_),
      width_(other.width_),
      height_(other.height_),
      attachments_(std::move(other.attachments_)) {
    other.fbo_ = 0;
    other.width_ = 0;
    other.height_ = 0;
}

RenderTarget& RenderTarget::operator=(RenderTarget&& other) noexcept {
    if (this != &other) {
        destroy();
        fbo_ = other.fbo_;
        width_ = other.width_;
        height_ = other.height_;
        attachments_ = std::move(other.attachments_);
        other.fbo_ = 0;
        other.width_ = 0;
        other.height_ = 0;
    }
    return *this;
}

void RenderTarget::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void RenderTarget::bind_default() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint RenderTarget::texture(GLenum attachment_point) const {
    auto it = std::find_if(attachments_.begin(), attachments_.end(),
        [attachment_point](const AttachmentData& a) { return a.point == attachment_point; });
    if (it != attachments_.end()) return it->texture;
    return 0;
}

void RenderTarget::destroy() {
    for (const auto& att : attachments_) {
        if (att.texture) glDeleteTextures(1, &att.texture);
    }
    attachments_.clear();
    if (fbo_) {
        glDeleteFramebuffers(1, &fbo_);
        fbo_ = 0;
    }
}
