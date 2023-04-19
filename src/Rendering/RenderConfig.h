#pragma once

class RenderConfig {
public:
    enum class ClearBit : unsigned int {
        Colour  = 1 << 0,
        Depth   = 1 << 1,
        Stencil = 1 << 2,
    };

    enum class BlendFuncSrc {
        One = 0,
        SrcColour,
        OneMinusSrcColour,
        DstColour,
        OneMinusDstColour,
        SrcAlpha,
        OneMinusSrcAlpha,
        ConstantColour,
        OneMinusConstantColour,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SrcAlphaSaturate,

        Max,
    };
    enum class BlendFuncDst {
        Zero = 0,
        One,
        SrcColour,
        OneMinusSrcColour,
        DstColour,
        OneMinusDstColour,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColour,
        OneMinusConstantColour,
        ConstantAlpha,
        OneMinusConstantAlpha,

        Max,
    };

    enum class CullFaceMode {
        Front,
        Back,
        FrontAndBack,

        Max,
    };

    enum class DepthTestFunc {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,

        Max,
    };

    static void setDefaultViewport(int x, int y, unsigned int width, unsigned int height);
    static void setViewport(int x = mVPortDefaultX, int y = mVPortDefaultY, unsigned int width = mVPortDefaultW, unsigned int height = mVPortDefaultH);

    static void setClearColour(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    static void clearBuffers(ClearBit mask);

    static void setBlend(bool enabled = false, BlendFuncSrc srcFactor = BlendFuncSrc::One, BlendFuncDst dstFactor = BlendFuncDst::Zero);
    static void setColourMask(bool red = true, bool green = true, bool blue = true, bool alpha = true);
    static void setCullFace(bool enabled = true, CullFaceMode mode = CullFaceMode::Back);
    static void setDepthTest(bool enabled = false, DepthTestFunc func = DepthTestFunc::Less, float nearVal = 0.0f, float farVal = 1.0f);
    static void setDepthMask(bool enabled = false);
    static void setPatchVertices(int numVertices = 3);
private:
    static int mVPortDefaultX, mVPortDefaultY;
    static unsigned int mVPortDefaultW, mVPortDefaultH;
};

bool operator&(RenderConfig::ClearBit a, RenderConfig::ClearBit b);

RenderConfig::ClearBit operator|(RenderConfig::ClearBit a, RenderConfig::ClearBit b);
