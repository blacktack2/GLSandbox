#include "RenderConfig.h"

#include <glad/glad.h>

int          RenderConfig::sVPortDefaultX = 0;
int          RenderConfig::sVPortDefaultY = 0;
unsigned int RenderConfig::sVPortDefaultW = 1024;
unsigned int RenderConfig::sVPortDefaultH = 1024;

std::vector<IResizeCallable*> RenderConfig::sResizeEvents{};

GLenum parseBlendFuncSrc(RenderConfig::BlendFuncSrc srcFactor) {
    switch (srcFactor) {
        default:
        case RenderConfig::BlendFuncSrc::One                    : return GL_ONE;
        case RenderConfig::BlendFuncSrc::SrcColour              : return GL_SRC_COLOR;
        case RenderConfig::BlendFuncSrc::OneMinusSrcColour      : return GL_ONE_MINUS_SRC_COLOR;
        case RenderConfig::BlendFuncSrc::DstColour              : return GL_DST_COLOR;
        case RenderConfig::BlendFuncSrc::OneMinusDstColour      : return GL_ONE_MINUS_DST_COLOR;
        case RenderConfig::BlendFuncSrc::SrcAlpha               : return GL_SRC_ALPHA;
        case RenderConfig::BlendFuncSrc::OneMinusSrcAlpha       : return GL_ONE_MINUS_SRC_ALPHA;
        case RenderConfig::BlendFuncSrc::ConstantColour         : return GL_CONSTANT_COLOR;
        case RenderConfig::BlendFuncSrc::OneMinusConstantColour : return GL_ONE_MINUS_CONSTANT_COLOR;
        case RenderConfig::BlendFuncSrc::ConstantAlpha          : return GL_CONSTANT_ALPHA;
        case RenderConfig::BlendFuncSrc::OneMinusConstantAlpha  : return GL_ONE_MINUS_CONSTANT_ALPHA;
        case RenderConfig::BlendFuncSrc::SrcAlphaSaturate       : return GL_SRC_ALPHA_SATURATE;
    }
}

GLenum parseBlendFuncDst(RenderConfig::BlendFuncDst dstFactor) {
    switch (dstFactor) {
        default:
        case RenderConfig::BlendFuncDst::Zero                   : return GL_ZERO;
        case RenderConfig::BlendFuncDst::One                    : return GL_ONE;
        case RenderConfig::BlendFuncDst::SrcColour              : return GL_SRC_COLOR;
        case RenderConfig::BlendFuncDst::OneMinusSrcColour      : return GL_ONE_MINUS_SRC_COLOR;
        case RenderConfig::BlendFuncDst::DstColour              : return GL_DST_COLOR;
        case RenderConfig::BlendFuncDst::OneMinusDstColour      : return GL_ONE_MINUS_DST_COLOR;
        case RenderConfig::BlendFuncDst::SrcAlpha               : return GL_SRC_ALPHA;
        case RenderConfig::BlendFuncDst::OneMinusSrcAlpha       : return GL_ONE_MINUS_SRC_ALPHA;
        case RenderConfig::BlendFuncDst::DstAlpha               : return GL_DST_ALPHA;
        case RenderConfig::BlendFuncDst::OneMinusDstAlpha       : return GL_ONE_MINUS_DST_ALPHA;
        case RenderConfig::BlendFuncDst::ConstantColour         : return GL_CONSTANT_COLOR;
        case RenderConfig::BlendFuncDst::OneMinusConstantColour : return GL_ONE_MINUS_CONSTANT_COLOR;
        case RenderConfig::BlendFuncDst::ConstantAlpha          : return GL_CONSTANT_ALPHA;
        case RenderConfig::BlendFuncDst::OneMinusConstantAlpha  : return GL_ONE_MINUS_CONSTANT_ALPHA;
    }
}

GLenum parseCullFaceMode(RenderConfig::CullFaceMode mode) {
    switch (mode) {
        default:
        case RenderConfig::CullFaceMode::Front        : return GL_FRONT;
        case RenderConfig::CullFaceMode::Back         : return GL_BACK;
        case RenderConfig::CullFaceMode::FrontAndBack : return GL_FRONT_AND_BACK;
    }
}

GLenum parseDepthTestFunc(RenderConfig::DepthTestFunc func) {
    switch (func) {
        default:
        case RenderConfig::DepthTestFunc::Never        : return GL_NEVER;
        case RenderConfig::DepthTestFunc::Less         : return GL_LESS;
        case RenderConfig::DepthTestFunc::Equal        : return GL_EQUAL;
        case RenderConfig::DepthTestFunc::LessEqual    : return GL_LEQUAL;
        case RenderConfig::DepthTestFunc::Greater      : return GL_GREATER;
        case RenderConfig::DepthTestFunc::NotEqual     : return GL_NOTEQUAL;
        case RenderConfig::DepthTestFunc::GreaterEqual : return GL_GEQUAL;
        case RenderConfig::DepthTestFunc::Always       : return GL_ALWAYS;
    }
}

void RenderConfig::addResizeCallable(IResizeCallable& callable) {
    sResizeEvents.push_back(&callable);
}

void RenderConfig::removeResizeCallable(IResizeCallable& callable) {
    sResizeEvents.erase(std::remove_if(sResizeEvents.begin(), sResizeEvents.end(),
        [&callable](const IResizeCallable* otherCallable) { return &callable == otherCallable; }), sResizeEvents.end());
}

void RenderConfig::setDefaultViewport(int x, int y, unsigned int width, unsigned int height) {
    sVPortDefaultX = x;
    sVPortDefaultY = y;
    sVPortDefaultW = width;
    sVPortDefaultH = height;
    for (auto& callable : sResizeEvents)
        callable->onResizeEvent();
}

void RenderConfig::setViewport(int x, int y, unsigned int width, unsigned int height) {
    glViewport(x, y, width, height);
}

void RenderConfig::setClearColour(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void RenderConfig::clearBuffers(RenderConfig::ClearBit mask) {
    GLbitfield bitMask = 0;
    if (mask & ClearBit::Colour)
        bitMask |= GL_COLOR_BUFFER_BIT;
    if (mask & ClearBit::Depth)
        bitMask |= GL_DEPTH_BUFFER_BIT;
    if (mask & ClearBit::Stencil)
        bitMask |= GL_STENCIL_BUFFER_BIT;
    glClear(bitMask);
}

void RenderConfig::setBlend(bool enabled, RenderConfig::BlendFuncSrc srcFactor, RenderConfig::BlendFuncDst dstFactor) {
    if (!enabled) {
        glDisable(GL_BLEND);
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(parseBlendFuncSrc(srcFactor), parseBlendFuncDst(dstFactor));
}

void RenderConfig::setColourMask(bool red, bool green, bool blue, bool alpha) {
    glColorMask(red, green, blue, alpha);
}

void RenderConfig::setCullFace(bool enabled, RenderConfig::CullFaceMode mode) {
    if (!enabled) {
        glDisable(GL_CULL_FACE);
        return;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(parseCullFaceMode(mode));
}

void RenderConfig::setDepthTest(bool enabled, RenderConfig::DepthTestFunc func, float nearVal, float farVal) {
    if (!enabled) {
        glDisable(GL_DEPTH_TEST);
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(parseDepthTestFunc(func));
    glDepthRange(nearVal, farVal);
}

void RenderConfig::setDepthMask(bool enabled) {
    glDepthMask(enabled);
}

void RenderConfig::setPatchVertices(int numVertices) {
    glPatchParameteri(GL_PATCH_VERTICES, numVertices);
}

bool operator&(RenderConfig::ClearBit a, RenderConfig::ClearBit b) {
    return (unsigned int)a & (unsigned int)b;
}

RenderConfig::ClearBit operator|(RenderConfig::ClearBit a, RenderConfig::ClearBit b) {
    return (RenderConfig::ClearBit)((unsigned int)a | (unsigned int)b);
}
