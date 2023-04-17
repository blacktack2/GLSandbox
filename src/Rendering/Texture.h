#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>

typedef unsigned long long tex_format_t;

static constexpr tex_format_t gR_BIT    = 1 << 0;
static constexpr tex_format_t gRG_BIT   = 1 << 1;
static constexpr tex_format_t gRGB_BIT  = 1 << 2;
static constexpr tex_format_t gRGBA_BIT = 1 << 3;

static constexpr tex_format_t gP8_BIT  = 1 << 4;
static constexpr tex_format_t gP16_BIT = 1 << 5;
static constexpr tex_format_t gP32_BIT = 1 << 6;

static constexpr tex_format_t gSIGN_BIT = 1 << 7;

static constexpr tex_format_t gINT_BIT = 1 << 8;
static constexpr tex_format_t gFLT_BIT = 1 << 9;

static constexpr tex_format_t gSRGB_BIT  = 1 << 10;
static constexpr tex_format_t gCOMPR_BIT = 1 << 11;
static constexpr tex_format_t gRGTC_BIT = 1 << 12;
static constexpr tex_format_t gBPTC_BIT = 1 << 13;

static constexpr tex_format_t gDCOMP_BIT = 1 << 30;
static constexpr tex_format_t gDSTEN_BIT = 1 << 31;

class Texture {
public:
    enum class InternalFormat : tex_format_t {
        DepthComponent = gDCOMP_BIT,
        DepthStencil   = gDSTEN_BIT,
        R    = gR_BIT,
        RG   = gRG_BIT,
        RGB  = gRGB_BIT,
        RGBA = gRGBA_BIT,

        R8       = gR_BIT | gP8_BIT,
        R8SNorm  = gR_BIT | gP8_BIT | gSIGN_BIT,
        R8I      = gR_BIT | gP8_BIT | gINT_BIT | gSIGN_BIT,
        R8UI     = gR_BIT | gP8_BIT | gINT_BIT,
        R16      = gR_BIT | gP16_BIT,
        R16SNorm = gR_BIT | gP16_BIT | gSIGN_BIT,
        R16F     = gR_BIT | gP16_BIT | gFLT_BIT,
        R16I     = gR_BIT | gP16_BIT | gINT_BIT | gSIGN_BIT,
        R16UI    = gR_BIT | gP16_BIT | gINT_BIT,
        R32F     = gR_BIT | gP32_BIT | gFLT_BIT,
        R32I     = gR_BIT | gP32_BIT | gINT_BIT | gSIGN_BIT,
        R32UI    = gR_BIT | gP32_BIT | gINT_BIT,

        RG8       = gRG_BIT | gP8_BIT,
        RG8SNorm  = gRG_BIT | gP8_BIT | gSIGN_BIT,
        RG8I      = gRG_BIT | gP8_BIT | gINT_BIT | gSIGN_BIT,
        RG8UI     = gRG_BIT | gP8_BIT | gINT_BIT,
        RG16      = gRG_BIT | gP16_BIT,
        RG16SNorm = gRG_BIT | gP16_BIT | gSIGN_BIT,
        RG16F     = gRG_BIT | gP16_BIT | gFLT_BIT,
        RG16I     = gRG_BIT | gP16_BIT | gINT_BIT | gSIGN_BIT,
        RG16UI    = gRG_BIT | gP16_BIT | gINT_BIT,
        RG32F     = gRG_BIT | gP32_BIT | gFLT_BIT,
        RG32I     = gRG_BIT | gP32_BIT | gINT_BIT | gSIGN_BIT,
        RG32UI    = gRG_BIT | gP32_BIT | gINT_BIT,

        RGB8       = gRGB_BIT | gP8_BIT,
        RGB8SNorm  = gRGB_BIT | gP8_BIT | gSIGN_BIT,
        SRGB8      = gRGB_BIT | gP8_BIT | gSRGB_BIT,
        RGB8I      = gRGB_BIT | gP8_BIT | gINT_BIT | gSIGN_BIT,
        RGB8UI     = gRGB_BIT | gP8_BIT | gINT_BIT,
        RGB16SNorm = gRGB_BIT | gP16_BIT | gSIGN_BIT,
        RGB16F     = gRGB_BIT | gP16_BIT | gFLT_BIT,
        RGB16I     = gRGB_BIT | gP16_BIT | gINT_BIT | gSIGN_BIT,
        RGB16UI    = gRGB_BIT | gP16_BIT | gINT_BIT,
        RGB32F     = gRGB_BIT | gP32_BIT | gFLT_BIT,
        RGB32I     = gRGB_BIT | gP32_BIT | gINT_BIT | gSIGN_BIT,
        RGB32UI    = gRGB_BIT | gP32_BIT | gINT_BIT,

        RGB4       = gRGB_BIT | (1ull << 32), // TODO implement these formats properly
        RGB5       = gRGB_BIT | (1ull << 33),
        RGB10      = gRGB_BIT | (1ull << 34),
        RGB12      = gRGB_BIT | (1ull << 35),
        RGB9E5     = gRGB_BIT | (1ull << 36),
        R3G3B2     = gRGB_BIT | (1ull << 37),
        R11G11B10F = gRGB_BIT | (1ull << 38),

        RGBA8      = gRGBA_BIT | gP8_BIT,
        RGBA8SNorm = gRGBA_BIT | gP8_BIT | gSIGN_BIT,
        SRGB8A8    = gRGBA_BIT | gP8_BIT | gSRGB_BIT,
        RGBA8I     = gRGBA_BIT | gP8_BIT | gINT_BIT | gSIGN_BIT,
        RGBA8UI    = gRGBA_BIT | gP8_BIT | gINT_BIT,
        RGBA16     = gRGBA_BIT | gP16_BIT,
        RGBA16F    = gRGBA_BIT | gP16_BIT | gFLT_BIT,
        RGBA16I    = gRGBA_BIT | gP16_BIT | gINT_BIT | gSIGN_BIT,
        RGBA16UI   = gRGBA_BIT | gP16_BIT | gINT_BIT,
        RGBA32F    = gRGBA_BIT | gP32_BIT | gFLT_BIT,
        RGBA32I    = gRGBA_BIT | gP32_BIT | gINT_BIT | gSIGN_BIT,
        RGBA32UI   = gRGBA_BIT | gP32_BIT | gINT_BIT,

        RGBA2      = gRGBA_BIT | (1ull << 39), // TODO implement these formats properly
        RGBA4      = gRGBA_BIT | (1ull << 40),
        RGB5A1     = gRGBA_BIT | (1ull << 41),
        RGB10A2    = gRGBA_BIT | (1ull << 42),
        RGB10A2UI  = gRGBA_BIT | (1ull << 43),
        RGBA12     = gRGBA_BIT | (1ull << 44),

        CompressedR        = gCOMPR_BIT | gR_BIT | gP8_BIT,
        CompressedR_RGTC1  = gCOMPR_BIT | gR_BIT | gP8_BIT | gRGTC_BIT,
        CompressedSR_RGTC1 = gCOMPR_BIT | gR_BIT | gP8_BIT | gRGTC_BIT | gSIGN_BIT,

        CompressedRG        = gCOMPR_BIT | gRG_BIT | gP8_BIT,
        CompressedRG_RGTC1  = gCOMPR_BIT | gRG_BIT | gP8_BIT | gRGTC_BIT,
        CompressedSRG_RGTC1 = gCOMPR_BIT | gRG_BIT | gP8_BIT | gRGTC_BIT | gSIGN_BIT,

        CompressedRGB         = gCOMPR_BIT | gRGB_BIT | gP8_BIT,
        CompressedSRGB        = gCOMPR_BIT | gRGB_BIT | gP8_BIT | gSRGB_BIT,
        CompressedRGB_BPTC_F  = gCOMPR_BIT | gRGB_BIT | gP8_BIT | gBPTC_BIT | gSIGN_BIT,
        CompressedRGB_BPTC_UF = gCOMPR_BIT | gRGB_BIT | gP8_BIT | gBPTC_BIT,

        CompressedRGBA             = gCOMPR_BIT | gRGBA_BIT | gP8_BIT,
        CompressedSRGBA            = gCOMPR_BIT | gRGBA_BIT | gP8_BIT | gSRGB_BIT,
        CompressedRGBA_BPTC_UNorm  = gCOMPR_BIT | gRGBA_BIT | gP8_BIT | gBPTC_BIT,
        CompressedSRGBA_BPTC_UNorm = gCOMPR_BIT | gRGBA_BIT | gP8_BIT | gBPTC_BIT | gSIGN_BIT,
    };
    enum class DataFormat : size_t {
        R,
        RG,
        RGB,
        BGR,
        RGBA,
        BGRA,
        RI,
        RGI,
        RGBI,
        BGRI,
        RGBAI,
        BGRAI,
        StencilIndex,
        DepthComponent,
        DepthStencil,

        Max,
    };
    enum class DataType {
        UnsignedByte,
        Byte,
        UnsignedShort,
        Short,
        UnsignedInt,
        Int,
        HalfFloat,
        Float,
        UnsignedByte_3_3_2,
        UnsignedByte_2_2_3_R,
        UnsignedShort_5_6_5,
        UnsignedShort_5_6_5_R,
        UnsignedShort_4_4_4_4,
        UnsignedShort_4_4_4_4_R,
        UnsignedShort_5_5_5_1,
        UnsignedShort_1_5_5_5_R,
        UnsignedInt_8_8_8_8,
        UnsignedInt_8_8_8_8_R,
        UnsignedInt_10_10_10_2,
        UnsignedInt_2_10_10_10_R,
    };

    enum class MinFilter : size_t {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear,

        Max,
    };
    enum class MagFilter : size_t {
        Nearest,
        Linear,

        Max,
    };

    enum class EdgeWrap : size_t {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,

        Max,
    };

    enum class ErrorState {
        INVALID,
        VALID,
    };

    Texture();
    ~Texture();

    void bind() const;
    void bind(int slot) const;
    static void unbind();

    void resize(int width, int height);
    void uploadData(void* data, DataFormat dataFormat, DataType dataType);

    inline void setInternalFormat(InternalFormat format) {
        mInternalFormat = format;
    }

    inline ErrorState getState() {
        return mState;
    }

    static void setFilters(MinFilter min, MagFilter mag);
    static void setEdgeWrap(EdgeWrap mode);
private:
    unsigned int mTexID = 0;

    int mWidth  = 1;
    int mHeight = 1;

    InternalFormat mInternalFormat = InternalFormat::RGBA32F;

    ErrorState mState = ErrorState::INVALID;
};

