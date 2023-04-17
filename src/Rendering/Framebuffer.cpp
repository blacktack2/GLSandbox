#include "Framebuffer.h"

#include "Texture.h"

#include <glad/glad.h>

static constexpr GLenum gCOLOUR_BUFFERS[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
    GL_COLOR_ATTACHMENT8,
    GL_COLOR_ATTACHMENT9,
};

Framebuffer::ErrorState parseErrorState(GLenum state) {
    switch (state) {
        default: return Framebuffer::ErrorState::Invalid;
        case GL_FRAMEBUFFER_COMPLETE                      : return Framebuffer::ErrorState::Valid;
        case GL_FRAMEBUFFER_UNDEFINED                     : return Framebuffer::ErrorState::Undefined;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT         : return Framebuffer::ErrorState::IncompleteAttachment;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : return Framebuffer::ErrorState::IncompleteMissingAttachment;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER        : return Framebuffer::ErrorState::IncompleteDrawBuffer;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER        : return Framebuffer::ErrorState::IncompleteReadBuffer;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE        : return Framebuffer::ErrorState::IncompleteMultiSample;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS      : return Framebuffer::ErrorState::IncompleteLayerTargets;
    }
}

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &mFBO);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &mFBO);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::reset() {
    mNumColourBuffers = 0;

    mErrorState = ErrorState::Invalid;
    glDrawBuffer(GL_NONE);
}

void Framebuffer::drawBuffers() {
    if (mNumColourBuffers == 0)
        glDrawBuffer(GL_NONE);
    else
        glDrawBuffers(mNumColourBuffers, gCOLOUR_BUFFERS);
    mErrorState = parseErrorState(glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

void Framebuffer::bindTexture(const Texture& texture) {
    GLenum attachmentType;
    switch (texture.getInternalFormat()) {
        default: attachmentType = GL_COLOR_ATTACHMENT0 + mNumColourBuffers++; break;
        case Texture::InternalFormat::DepthComponent : attachmentType = GL_DEPTH_ATTACHMENT; break;
        case Texture::InternalFormat::DepthStencil   : attachmentType = GL_DEPTH_STENCIL_ATTACHMENT; break;
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, texture.getID(), 0);
}

void Framebuffer::bindTexture(const Texture& texture, unsigned int attachment) {
    mNumColourBuffers = std::max(mNumColourBuffers, attachment + 1);
    GLenum attachmentType;
    switch (texture.getInternalFormat()) {
        default: attachmentType = GL_COLOR_ATTACHMENT0 + attachment; break;
        case Texture::InternalFormat::DepthComponent : attachmentType = GL_DEPTH_ATTACHMENT; break;
        case Texture::InternalFormat::DepthStencil   : attachmentType = GL_DEPTH_STENCIL_ATTACHMENT; break;
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, texture.getID(), 0);
}
