#include "Texture.h"

GLint parseInternalFormat(Texture::InternalFormat format) {
    switch (format) {
        default:
        case Texture::InternalFormat::DepthComponent             : return GL_DEPTH_COMPONENT;
        case Texture::InternalFormat::DepthStencil               : return GL_DEPTH_STENCIL;
        case Texture::InternalFormat::R                          : return GL_RED;
        case Texture::InternalFormat::RG                         : return GL_RG;
        case Texture::InternalFormat::RGB                        : return GL_RGB;
        case Texture::InternalFormat::RGBA                       : return GL_RGBA;
        case Texture::InternalFormat::R8                         : return GL_R8;
        case Texture::InternalFormat::R8SNorm                    : return GL_R8_SNORM;
        case Texture::InternalFormat::R16                        : return GL_R16;
        case Texture::InternalFormat::R16SNorm                   : return GL_R16_SNORM;
        case Texture::InternalFormat::RG8                        : return GL_RG8;
        case Texture::InternalFormat::RG8SNorm                   : return GL_RG8_SNORM;
        case Texture::InternalFormat::RG16                       : return GL_RG16;
        case Texture::InternalFormat::RG16SNorm                  : return GL_RG16_SNORM;
        case Texture::InternalFormat::R3G3B2                     : return GL_R3_G3_B2;
        case Texture::InternalFormat::RGB4                       : return GL_RGB4;
        case Texture::InternalFormat::RGB5                       : return GL_RGB5;
        case Texture::InternalFormat::RGB8                       : return GL_RGB8;
        case Texture::InternalFormat::RGB8SNorm                  : return GL_RGB8_SNORM;
        case Texture::InternalFormat::RGB10                      : return GL_RGB10;
        case Texture::InternalFormat::RGB12                      : return GL_RGB12;
        case Texture::InternalFormat::RGB16SNorm                 : return GL_RGB16_SNORM;
        case Texture::InternalFormat::RGBA2                      : return GL_RGBA2;
        case Texture::InternalFormat::RGBA4                      : return GL_RGBA4;
        case Texture::InternalFormat::RGB5A1                     : return GL_RGB5_A1;
        case Texture::InternalFormat::RGBA8                      : return GL_RGBA8;
        case Texture::InternalFormat::RGBA8SNorm                 : return GL_RGBA8_SNORM;
        case Texture::InternalFormat::RGB10A2                    : return GL_RGB10_A2;
        case Texture::InternalFormat::RGB10A2UI                  : return GL_RGB10_A2UI;
        case Texture::InternalFormat::RGBA12                     : return GL_RGBA12;
        case Texture::InternalFormat::RGBA16                     : return GL_RGBA16;
        case Texture::InternalFormat::SRGB8                      : return GL_SRGB8;
        case Texture::InternalFormat::SRGB8A8                    : return GL_SRGB8_ALPHA8;
        case Texture::InternalFormat::R16F                       : return GL_R16F;
        case Texture::InternalFormat::RG16F                      : return GL_RG16F;
        case Texture::InternalFormat::RGB16F                     : return GL_RGB16F;
        case Texture::InternalFormat::RGBA16F                    : return GL_RGBA16F;
        case Texture::InternalFormat::R32F                       : return GL_R32F;
        case Texture::InternalFormat::RG32F                      : return GL_RG32F;
        case Texture::InternalFormat::RGB32F                     : return GL_RGB32F;
        case Texture::InternalFormat::RGBA32F                    : return GL_RGBA32F;
        case Texture::InternalFormat::R11G11B10F                 : return GL_R11F_G11F_B10F;
        case Texture::InternalFormat::RGB9E5                     : return GL_RGB9_E5;
        case Texture::InternalFormat::R8I                        : return GL_R8I;
        case Texture::InternalFormat::R8UI                       : return GL_R8UI;
        case Texture::InternalFormat::R16I                       : return GL_R16I;
        case Texture::InternalFormat::R16UI                      : return GL_R16UI;
        case Texture::InternalFormat::R32I                       : return GL_R32I;
        case Texture::InternalFormat::R32UI                      : return GL_R32UI;
        case Texture::InternalFormat::RG8I                       : return GL_RG8I;
        case Texture::InternalFormat::RG8UI                      : return GL_RG8UI;
        case Texture::InternalFormat::RG16I                      : return GL_RG16I;
        case Texture::InternalFormat::RG16UI                     : return GL_RG16UI;
        case Texture::InternalFormat::RG32I                      : return GL_RG32I;
        case Texture::InternalFormat::RG32UI                     : return GL_RG32UI;
        case Texture::InternalFormat::RGB8I                      : return GL_RGB8I;
        case Texture::InternalFormat::RGB8UI                     : return GL_RGB8UI;
        case Texture::InternalFormat::RGB16I                     : return GL_RGB16I;
        case Texture::InternalFormat::RGB16UI                    : return GL_RGB16UI;
        case Texture::InternalFormat::RGB32I                     : return GL_RGB32I;
        case Texture::InternalFormat::RGB32UI                    : return GL_RGB32UI;
        case Texture::InternalFormat::RGBA8I                     : return GL_RGBA8I;
        case Texture::InternalFormat::RGBA8UI                    : return GL_RGBA8UI;
        case Texture::InternalFormat::RGBA16I                    : return GL_RGBA16I;
        case Texture::InternalFormat::RGBA16UI                   : return GL_RGBA16UI;
        case Texture::InternalFormat::RGBA32I                    : return GL_RGBA32I;
        case Texture::InternalFormat::RGBA32UI                   : return GL_RGBA32UI;
        case Texture::InternalFormat::CompressedR                : return GL_COMPRESSED_RED;
        case Texture::InternalFormat::CompressedRG               : return GL_COMPRESSED_RG;
        case Texture::InternalFormat::CompressedRGB              : return GL_COMPRESSED_RGB;
        case Texture::InternalFormat::CompressedRGBA             : return GL_COMPRESSED_RGBA;
        case Texture::InternalFormat::CompressedSRGB             : return GL_COMPRESSED_SRGB;
        case Texture::InternalFormat::CompressedSRGBA            : return GL_COMPRESSED_SRGB_ALPHA;
        case Texture::InternalFormat::CompressedR_RGTC1          : return GL_COMPRESSED_RED_RGTC1;
        case Texture::InternalFormat::CompressedSR_RGTC1         : return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case Texture::InternalFormat::CompressedRG_RGTC1         : return GL_COMPRESSED_RG_RGTC2;
        case Texture::InternalFormat::CompressedSRG_RGTC1        : return GL_COMPRESSED_SIGNED_RG_RGTC2;
        case Texture::InternalFormat::CompressedRGBA_BPTC_UNorm  : return GL_COMPRESSED_RGBA_BPTC_UNORM;
        case Texture::InternalFormat::CompressedSRGBA_BPTC_UNorm : return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
        case Texture::InternalFormat::CompressedRGB_BPTC_F       : return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
        case Texture::InternalFormat::CompressedRGB_BPTC_UF      : return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
    }
}

GLenum parseDataFormat(Texture::DataFormat format) {
    switch (format) {
        default:
        case Texture::DataFormat::R              : return GL_RED;
        case Texture::DataFormat::RG             : return GL_RG;
        case Texture::DataFormat::RGB            : return GL_RGB;
        case Texture::DataFormat::BGR            : return GL_BGR;
        case Texture::DataFormat::RGBA           : return GL_RGBA;
        case Texture::DataFormat::BGRA           : return GL_BGRA;
        case Texture::DataFormat::RI             : return GL_RED_INTEGER;
        case Texture::DataFormat::RGI            : return GL_RG_INTEGER;
        case Texture::DataFormat::RGBI           : return GL_RGB_INTEGER;
        case Texture::DataFormat::BGRI           : return GL_BGR_INTEGER;
        case Texture::DataFormat::RGBAI          : return GL_RGBA_INTEGER;
        case Texture::DataFormat::BGRAI          : return GL_BGRA_INTEGER;
        case Texture::DataFormat::StencilIndex   : return GL_STENCIL_INDEX;
        case Texture::DataFormat::DepthComponent : return GL_DEPTH_COMPONENT;
        case Texture::DataFormat::DepthStencil   : return GL_DEPTH_STENCIL;
    }
}

GLenum parseDataType(Texture::DataType type) {
    switch (type) {
        default:
        case Texture::DataType::UnsignedByte             : return GL_UNSIGNED_BYTE;
        case Texture::DataType::Byte                     : return GL_BYTE;
        case Texture::DataType::UnsignedShort            : return GL_UNSIGNED_SHORT;
        case Texture::DataType::Short                    : return GL_SHORT;
        case Texture::DataType::UnsignedInt              : return GL_UNSIGNED_INT;
        case Texture::DataType::Int                      : return GL_INT;
        case Texture::DataType::HalfFloat                : return GL_HALF_FLOAT;
        case Texture::DataType::Float                    : return GL_FLOAT;
        case Texture::DataType::UnsignedByte_3_3_2       : return GL_UNSIGNED_BYTE_3_3_2;
        case Texture::DataType::UnsignedByte_2_2_3_R     : return GL_UNSIGNED_BYTE_2_3_3_REV;
        case Texture::DataType::UnsignedShort_5_6_5      : return GL_UNSIGNED_SHORT_5_6_5;
        case Texture::DataType::UnsignedShort_5_6_5_R    : return GL_UNSIGNED_SHORT_5_6_5_REV;
        case Texture::DataType::UnsignedShort_4_4_4_4    : return GL_UNSIGNED_SHORT_4_4_4_4;
        case Texture::DataType::UnsignedShort_4_4_4_4_R  : return GL_UNSIGNED_SHORT_4_4_4_4_REV;
        case Texture::DataType::UnsignedShort_5_5_5_1    : return GL_UNSIGNED_SHORT_5_5_5_1;
        case Texture::DataType::UnsignedShort_1_5_5_5_R  : return GL_UNSIGNED_SHORT_1_5_5_5_REV;
        case Texture::DataType::UnsignedInt_8_8_8_8      : return GL_UNSIGNED_INT_8_8_8_8;
        case Texture::DataType::UnsignedInt_8_8_8_8_R    : return GL_UNSIGNED_INT_8_8_8_8_REV;
        case Texture::DataType::UnsignedInt_10_10_10_2   : return GL_UNSIGNED_INT_10_10_10_2;
        case Texture::DataType::UnsignedInt_2_10_10_10_R : return GL_UNSIGNED_INT_2_10_10_10_REV;
    }
}

GLenum parseMagFilter(Texture::MagFilter filter) {
    switch (filter) {
        default:
        case Texture::MagFilter::Nearest : return GL_NEAREST;
        case Texture::MagFilter::Linear  : return GL_LINEAR;
    }
}

GLfloat parseMinFilter(Texture::MinFilter filter) {
    switch (filter) {
        default:
        case Texture::MinFilter::Nearest              : return GL_NEAREST;
        case Texture::MinFilter::Linear               : return GL_LINEAR;
        case Texture::MinFilter::NearestMipmapNearest : return GL_NEAREST_MIPMAP_NEAREST;
        case Texture::MinFilter::LinearMipmapNearest  : return GL_LINEAR_MIPMAP_NEAREST;
        case Texture::MinFilter::NearestMipmapLinear  : return GL_NEAREST_MIPMAP_LINEAR;
        case Texture::MinFilter::LinearMipmapLinear   : return GL_LINEAR_MIPMAP_LINEAR;
    }
}

GLfloat parseEdgeWrap(Texture::EdgeWrap mode) {
    switch (mode) {
        default:
        case Texture::EdgeWrap::Repeat         : return GL_REPEAT;
        case Texture::EdgeWrap::MirroredRepeat : return GL_MIRRORED_REPEAT;
        case Texture::EdgeWrap::ClampToEdge    : return GL_CLAMP_TO_EDGE;
        case Texture::EdgeWrap::ClampToBorder  : return GL_CLAMP_TO_BORDER;
    }
}

void generateDummyTypes(Texture::InternalFormat internalFormat,
                        Texture::DataFormat& dummyFormat, Texture::DataType& dummyType) {
    switch (internalFormat) {
        default:
        case Texture::InternalFormat::DepthComponent:
            dummyFormat = Texture::DataFormat::DepthComponent;
            dummyType   = Texture::DataType::Float;
            break;
        case Texture::InternalFormat::DepthStencil:
            dummyFormat = Texture::DataFormat::StencilIndex;
            dummyType   = Texture::DataType::UnsignedByte;
            break;
        case Texture::InternalFormat::R:
        case Texture::InternalFormat::R8:
        case Texture::InternalFormat::R8SNorm:
        case Texture::InternalFormat::R16:
        case Texture::InternalFormat::R16SNorm:
        case Texture::InternalFormat::CompressedR:
        case Texture::InternalFormat::CompressedR_RGTC1:
            dummyFormat = Texture::DataFormat::R;
            dummyType   = Texture::DataType::UnsignedByte;
            break;
        case Texture::InternalFormat::RG:
        case Texture::InternalFormat::RG8:
        case Texture::InternalFormat::RG8SNorm:
        case Texture::InternalFormat::RG16:
        case Texture::InternalFormat::RG16SNorm:
        case Texture::InternalFormat::CompressedRG:
        case Texture::InternalFormat::CompressedRG_RGTC1:
            dummyFormat = Texture::DataFormat::RG;
            dummyType   = Texture::DataType::UnsignedByte;
            break;
        case Texture::InternalFormat::RGB:
        case Texture::InternalFormat::R3G3B2:
        case Texture::InternalFormat::RGB4:
        case Texture::InternalFormat::RGB5:
        case Texture::InternalFormat::RGB8:
        case Texture::InternalFormat::RGB8SNorm:
        case Texture::InternalFormat::RGB10:
        case Texture::InternalFormat::RGB12:
        case Texture::InternalFormat::RGB16SNorm:
        case Texture::InternalFormat::SRGB8:
        case Texture::InternalFormat::CompressedRGB:
        case Texture::InternalFormat::CompressedSRGB:
            dummyFormat = Texture::DataFormat::RGB;
            dummyType   = Texture::DataType::UnsignedByte;
            break;
        case Texture::InternalFormat::RGBA:
        case Texture::InternalFormat::RGBA2:
        case Texture::InternalFormat::RGBA4:
        case Texture::InternalFormat::RGB5A1:
        case Texture::InternalFormat::RGBA8:
        case Texture::InternalFormat::RGBA8SNorm:
        case Texture::InternalFormat::RGB10A2:
        case Texture::InternalFormat::RGB10A2UI:
        case Texture::InternalFormat::RGBA12:
        case Texture::InternalFormat::RGBA16:
        case Texture::InternalFormat::SRGB8A8:
        case Texture::InternalFormat::CompressedRGBA:
        case Texture::InternalFormat::CompressedSRGBA:
        case Texture::InternalFormat::CompressedRGBA_BPTC_UNorm:
        case Texture::InternalFormat::CompressedSRGBA_BPTC_UNorm:
            dummyFormat = Texture::DataFormat::RGBA;
            dummyType   = Texture::DataType::UnsignedByte;
            break;
        case Texture::InternalFormat::R16F:
        case Texture::InternalFormat::R32F:
            dummyFormat = Texture::DataFormat::R;
            dummyType   = Texture::DataType::Float;
            break;
        case Texture::InternalFormat::RG16F:
        case Texture::InternalFormat::RG32F:
            dummyFormat = Texture::DataFormat::RG;
            dummyType   = Texture::DataType::Float;
            break;
        case Texture::InternalFormat::RGB16F:
        case Texture::InternalFormat::RGB32F:
        case Texture::InternalFormat::R11G11B10F:
        case Texture::InternalFormat::RGB9E5:
        case Texture::InternalFormat::CompressedRGB_BPTC_F:
        case Texture::InternalFormat::CompressedRGB_BPTC_UF:
            dummyFormat = Texture::DataFormat::RGB;
            dummyType   = Texture::DataType::Float;
            break;
        case Texture::InternalFormat::RGBA16F:
        case Texture::InternalFormat::RGBA32F:
            dummyFormat = Texture::DataFormat::RGBA;
            dummyType   = Texture::DataType::Float;
            break;
        case Texture::InternalFormat::R8I:
        case Texture::InternalFormat::R16I:
        case Texture::InternalFormat::R32I:
            dummyFormat = Texture::DataFormat::RI;
            dummyType   = Texture::DataType::Int;
            break;
        case Texture::InternalFormat::R8UI:
        case Texture::InternalFormat::R16UI:
        case Texture::InternalFormat::R32UI:
            dummyFormat = Texture::DataFormat::RI;
            dummyType   = Texture::DataType::UnsignedInt;
            break;
        case Texture::InternalFormat::RG8I:
        case Texture::InternalFormat::RG16I:
        case Texture::InternalFormat::RG32I:
            dummyFormat = Texture::DataFormat::RGI;
            dummyType   = Texture::DataType::Int;
            break;
        case Texture::InternalFormat::RG8UI:
        case Texture::InternalFormat::RG16UI:
        case Texture::InternalFormat::RG32UI:
            dummyFormat = Texture::DataFormat::RGI;
            dummyType   = Texture::DataType::UnsignedInt;
            break;
        case Texture::InternalFormat::RGB8I:
        case Texture::InternalFormat::RGB16I:
        case Texture::InternalFormat::RGB32I:
            dummyFormat = Texture::DataFormat::RGBI;
            dummyType   = Texture::DataType::Int;
            break;
        case Texture::InternalFormat::RGB8UI:
        case Texture::InternalFormat::RGB16UI:
        case Texture::InternalFormat::RGB32UI:
            dummyFormat = Texture::DataFormat::RGBI;
            dummyType   = Texture::DataType::UnsignedInt;
            break;
        case Texture::InternalFormat::RGBA8I:
        case Texture::InternalFormat::RGBA16I:
        case Texture::InternalFormat::RGBA32I:
            dummyFormat = Texture::DataFormat::RGBAI;
            dummyType   = Texture::DataType::Int;
            break;
        case Texture::InternalFormat::RGBA8UI:
        case Texture::InternalFormat::RGBA16UI:
        case Texture::InternalFormat::RGBA32UI:
            dummyFormat = Texture::DataFormat::RGBAI;
            dummyType   = Texture::DataType::UnsignedInt;
            break;
        case Texture::InternalFormat::CompressedSR_RGTC1:
            dummyFormat = Texture::DataFormat::R;
            dummyType   = Texture::DataType::Byte;
            break;
        case Texture::InternalFormat::CompressedSRG_RGTC1:
            dummyFormat = Texture::DataFormat::RG;
            dummyType   = Texture::DataType::Byte;
            break;
    }
}

Texture::Texture() {
    glGenTextures(1, &mTexID);
}

Texture::~Texture() {
    glDeleteTextures(1, &mTexID);
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, mTexID);
}

void Texture::bind(int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, mTexID);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::resize(int width, int height) {
    mWidth = width;
    mHeight = height;

    Texture::DataFormat dummyFormat;
    Texture::DataType dummyType;
    generateDummyTypes(mInternalFormat, dummyFormat, dummyType);

    glTexImage2D(GL_TEXTURE_2D, 0, parseInternalFormat(mInternalFormat), mWidth, mHeight, 0,
                 parseDataFormat(dummyFormat), parseDataType(dummyType), nullptr);
    mState = (glIsTexture(mTexID) == GL_TRUE) ? ErrorState::VALID : ErrorState::INVALID;
}

void Texture::uploadData(void* data, DataFormat dataFormat, DataType dataType) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, parseDataFormat(dataFormat), parseDataType(dataType), data);
    mState = (glIsTexture(mTexID) == GL_TRUE) ? ErrorState::VALID : ErrorState::INVALID;
}

void Texture::setFilters(Texture::MinFilter min, Texture::MagFilter mag) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parseMinFilter(min));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parseMagFilter(mag));
}

void Texture::setEdgeWrap(Texture::EdgeWrap mode) {
    GLfloat edgeWrap = parseEdgeWrap(mode);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeWrap);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeWrap);
}
