#include "TextureNode.h"

#include "../../Utils/SerializationUtils.h"

#include <sstream>

Texture::InternalFormat parseInternalFormat(size_t numChannels, TextureNode::DataType dataType, TextureNode::Precision precision,
                                            bool isSigned, bool isSRGB, bool isCompressed) {
    tex_format_t format = 0;

    if (isSigned)
        format |= gSIGN_BIT;
    if (isSRGB)
        format |= gSRGB_BIT;
    if (isCompressed) {
        format |= gCOMPR_BIT;
    }

    switch (numChannels) {
        default: break;
        case 1: format |= gR_BIT; break;
        case 2: format |= gRG_BIT; break;
        case 3: format |= gRGB_BIT; break;
        case 4: format |= gRGBA_BIT; break;
    }

    switch (dataType) {
        default:
        case TextureNode::DataType::Clamp : break;
        case TextureNode::DataType::Int   : format |= gINT_BIT; break;
        case TextureNode::DataType::Float : format |= gFLT_BIT; break;
    }

    switch (precision) {
        default: break;
        case TextureNode::Precision::_8: format |= gP8_BIT; break;
        case TextureNode::Precision::_16: format |= gP16_BIT; break;
        case TextureNode::Precision::_32: format |= gP32_BIT; break;
    }

    return (Texture::InternalFormat)format;
}

std::string getInternalFormatLabel(Texture::InternalFormat format) {
    switch (format) {
        default: return "Unsupported Format";
        case Texture::InternalFormat::DepthComponent             : return "DEPTH_COMPONENT";
        case Texture::InternalFormat::DepthStencil               : return "DEPTH_STENCIL(Not Implemented)";
        case Texture::InternalFormat::R                          : return "RED";
        case Texture::InternalFormat::RG                         : return "RG";
        case Texture::InternalFormat::RGB                        : return "RGB";
        case Texture::InternalFormat::RGBA                       : return "RGBA";
        case Texture::InternalFormat::R8                         : return "R8";
        case Texture::InternalFormat::R8SNorm                    : return "R8_SNORM";
        case Texture::InternalFormat::R16                        : return "R16";
        case Texture::InternalFormat::R16SNorm                   : return "R16_SNORM";
        case Texture::InternalFormat::RG8                        : return "RG8";
        case Texture::InternalFormat::RG8SNorm                   : return "RG8_SNORM";
        case Texture::InternalFormat::RG16                       : return "RG16";
        case Texture::InternalFormat::RG16SNorm                  : return "RG16_SNORM";
        case Texture::InternalFormat::R3G3B2                     : return "R3_G3_B2";
        case Texture::InternalFormat::RGB4                       : return "RGB4";
        case Texture::InternalFormat::RGB5                       : return "RGB5";
        case Texture::InternalFormat::RGB8                       : return "RGB8";
        case Texture::InternalFormat::RGB8SNorm                  : return "RGB8_SNORM";
        case Texture::InternalFormat::RGB10                      : return "RGB10";
        case Texture::InternalFormat::RGB12                      : return "RGB12";
        case Texture::InternalFormat::RGB16SNorm                 : return "RGB16_SNORM";
        case Texture::InternalFormat::RGBA2                      : return "RGBA2";
        case Texture::InternalFormat::RGBA4                      : return "RGBA4";
        case Texture::InternalFormat::RGB5A1                     : return "RGB5_A1";
        case Texture::InternalFormat::RGBA8                      : return "RGBA8";
        case Texture::InternalFormat::RGBA8SNorm                 : return "RGBA8_SNORM";
        case Texture::InternalFormat::RGB10A2                    : return "RGB10_A2";
        case Texture::InternalFormat::RGB10A2UI                  : return "RGB10_A2UI";
        case Texture::InternalFormat::RGBA12                     : return "RGBA12";
        case Texture::InternalFormat::RGBA16                     : return "RGBA16";
        case Texture::InternalFormat::SRGB8                      : return "SRGB8";
        case Texture::InternalFormat::SRGB8A8                    : return "SRGB8_ALPHA8";
        case Texture::InternalFormat::R16F                       : return "R16F";
        case Texture::InternalFormat::RG16F                      : return "RG16F";
        case Texture::InternalFormat::RGB16F                     : return "RGB16F";
        case Texture::InternalFormat::RGBA16F                    : return "RGBA16F";
        case Texture::InternalFormat::R32F                       : return "R32F";
        case Texture::InternalFormat::RG32F                      : return "RG32F";
        case Texture::InternalFormat::RGB32F                     : return "RGB32F";
        case Texture::InternalFormat::RGBA32F                    : return "RGBA32F";
        case Texture::InternalFormat::R11G11B10F                 : return "R11F_G11F_B10F";
        case Texture::InternalFormat::RGB9E5                     : return "RGB9_E5";
        case Texture::InternalFormat::R8I                        : return "R8I";
        case Texture::InternalFormat::R8UI                       : return "R8UI";
        case Texture::InternalFormat::R16I                       : return "R16I";
        case Texture::InternalFormat::R16UI                      : return "R16UI";
        case Texture::InternalFormat::R32I                       : return "R32I";
        case Texture::InternalFormat::R32UI                      : return "R32UI";
        case Texture::InternalFormat::RG8I                       : return "RG8I";
        case Texture::InternalFormat::RG8UI                      : return "RG8UI";
        case Texture::InternalFormat::RG16I                      : return "RG16I";
        case Texture::InternalFormat::RG16UI                     : return "RG16UI";
        case Texture::InternalFormat::RG32I                      : return "RG32I";
        case Texture::InternalFormat::RG32UI                     : return "RG32UI";
        case Texture::InternalFormat::RGB8I                      : return "RGB8I";
        case Texture::InternalFormat::RGB8UI                     : return "RGB8UI";
        case Texture::InternalFormat::RGB16I                     : return "RGB16I";
        case Texture::InternalFormat::RGB16UI                    : return "RGB16UI";
        case Texture::InternalFormat::RGB32I                     : return "RGB32I";
        case Texture::InternalFormat::RGB32UI                    : return "RGB32UI";
        case Texture::InternalFormat::RGBA8I                     : return "RGBA8I";
        case Texture::InternalFormat::RGBA8UI                    : return "RGBA8UI";
        case Texture::InternalFormat::RGBA16I                    : return "RGBA16I";
        case Texture::InternalFormat::RGBA16UI                   : return "RGBA16UI";
        case Texture::InternalFormat::RGBA32I                    : return "RGBA32I";
        case Texture::InternalFormat::RGBA32UI                   : return "RGBA32UI";
        case Texture::InternalFormat::CompressedR                : return "COMPRESSED_RED";
        case Texture::InternalFormat::CompressedRG               : return "COMPRESSED_RG";
        case Texture::InternalFormat::CompressedRGB              : return "COMPRESSED_RGB";
        case Texture::InternalFormat::CompressedRGBA             : return "COMPRESSED_RGBA";
        case Texture::InternalFormat::CompressedSRGB             : return "COMPRESSED_SRGB";
        case Texture::InternalFormat::CompressedSRGBA            : return "COMPRESSED_SRGB_ALPHA";
        case Texture::InternalFormat::CompressedR_RGTC1          : return "COMPRESSED_RED_RGTC1";
        case Texture::InternalFormat::CompressedSR_RGTC1         : return "COMPRESSED_SIGNED_RED_RGTC1";
        case Texture::InternalFormat::CompressedRG_RGTC1         : return "COMPRESSED_RG_RGTC2";
        case Texture::InternalFormat::CompressedSRG_RGTC1        : return "COMPRESSED_SIGNED_RG_RGTC2";
        case Texture::InternalFormat::CompressedRGBA_BPTC_UNorm  : return "COMPRESSED_RGBA_BPTC_UNORM";
        case Texture::InternalFormat::CompressedSRGBA_BPTC_UNorm : return "COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
        case Texture::InternalFormat::CompressedRGB_BPTC_F       : return "COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
        case Texture::InternalFormat::CompressedRGB_BPTC_UF      : return "COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
    }
}
std::string getDataTypeLabel(TextureNode::DataType type) {
    switch (type) {
        default: return "Undefined";
        case TextureNode::DataType::Clamp        : return "Clamped-Float";
        case TextureNode::DataType::Int          : return "Integer";
        case TextureNode::DataType::Float        : return "Float";
    }
}

std::string getPrecisionLabel(TextureNode::Precision precision) {
    switch (precision) {
        default: return "Undefined";
        case TextureNode::Precision::_8  : return "8";
        case TextureNode::Precision::_16 : return "16";
        case TextureNode::Precision::_32 : return "32";
    }
}

std::string getMagFilterLabel(Texture::MagFilter filter) {
    switch (filter) {
        default: return "Undefined";
        case Texture::MagFilter::Nearest : return "Nearest";
        case Texture::MagFilter::Linear  : return "Linear";
    }
}

std::string getMinFilterLabel(Texture::MinFilter filter) {
    switch (filter) {
        default: return "Undefined";
        case Texture::MinFilter::Nearest              : return "Nearest";
        case Texture::MinFilter::Linear               : return "Linear";
        case Texture::MinFilter::NearestMipmapNearest : return "Near-mip-Near";
        case Texture::MinFilter::LinearMipmapNearest  : return "Lin-mip-Near";
        case Texture::MinFilter::NearestMipmapLinear  : return "Near-mip-Lin";
        case Texture::MinFilter::LinearMipmapLinear   : return "Lin-mip-Lin";
    }
}

std::string getEdgeWrapLabel(Texture::EdgeWrap mode) {
    switch (mode) {
        default: return "Undefined";
        case Texture::EdgeWrap::Repeat         : return "Repeat";
        case Texture::EdgeWrap::MirroredRepeat : return "Repeat-Mirror";
        case Texture::EdgeWrap::ClampToEdge    : return "Clamp-Edge";
        case Texture::EdgeWrap::ClampToBorder  : return "Clamp-Border";
    }
}

std::string getCompareModeLabel(Texture::CompareMode mode) {
    switch (mode) {
        default: return "Undefined";
        case Texture::CompareMode::None                : return "None";
        case Texture::CompareMode::CompareRefToTexture : return "Compare-Ref";
    }
}

std::string getTextureTypeLabel(TextureNode::TextureType type) {
    switch (type) {
        default: return "Undefined";
        case TextureNode::TextureType::Colour  : return "Colour";
        case TextureNode::TextureType::Depth   : return "Depth";
        case TextureNode::TextureType::Stencil : return "Stencil";
    }
}

void getTextureStatus(Texture::ErrorState state, std::string& message, ImVec4& messageColour) {
    switch (state) {
        case Texture::ErrorState::INVALID:
            message = "Invalid Texture";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Texture::ErrorState::VALID:
            message = "Texture loaded";
            messageColour = ImVec4(0, 1, 0, 1);
            break;
    }
}

TextureNode::TextureNode() : Node("Texture") {
    mTexture = std::make_unique<Texture>();
    mTexture->bind();
    mTexture->resize(mTexBounds.x, mTexBounds.y);

    addPort(mTextureOut);
}

TextureNode::~TextureNode() {
    if (mIsScreenLocked)
        RenderConfig::removeResizeCallable(*this);
}

bool TextureNode::isValid() const {
    return mTexture->getState() == Texture::ErrorState::VALID;
}

void TextureNode::onResizeEvent() {
    updateTexture();
}

std::vector<std::pair<std::string, std::string>> TextureNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Type", SerializationUtils::serializeData((int)mTextureType));
    data.emplace_back("ScreenLock", SerializationUtils::serializeData(mIsScreenLocked));
    data.emplace_back("Bounds", SerializationUtils::serializeData(mTexBounds));
    data.emplace_back("Min", SerializationUtils::serializeData((int)mMinFilter));
    data.emplace_back("Mag", SerializationUtils::serializeData((int)mMagFilter));
    data.emplace_back("Wrap", SerializationUtils::serializeData((int)mEdgeWrap));
    data.emplace_back("Channels", SerializationUtils::serializeData(mNumChannels));
    data.emplace_back("DataType", SerializationUtils::serializeData((int)mDataType));
    data.emplace_back("Precision", SerializationUtils::serializeData((int)mPrecision));
    data.emplace_back("Signed", SerializationUtils::serializeData(mIsSigned));
    data.emplace_back("SRGB", SerializationUtils::serializeData(mIsSRGB));
    data.emplace_back("Compressed", SerializationUtils::serializeData(mIsCompressed));
    return data;
}

void TextureNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Type")
        SerializationUtils::deserializeData(stream, (int&)mTextureType);
    else if (dataID == "ScreenLock")
        SerializationUtils::deserializeData(stream, mIsScreenLocked);
    else if (dataID == "Bounds")
        SerializationUtils::deserializeData(stream, mTexBounds);
    else if (dataID == "Min")
        SerializationUtils::deserializeData(stream, (int&)mMinFilter);
    else if (dataID == "Mag")
        SerializationUtils::deserializeData(stream, (int&)mMagFilter);
    else if (dataID == "Wrap")
        SerializationUtils::deserializeData(stream, (int&)mEdgeWrap);
    else if (dataID == "Channels")
        SerializationUtils::deserializeData(stream, mNumChannels);
    else if (dataID == "DataType")
        SerializationUtils::deserializeData(stream, (int&)mDataType);
    else if (dataID == "Precision")
        SerializationUtils::deserializeData(stream, (int&)mPrecision);
    else if (dataID == "Signed")
        SerializationUtils::deserializeData(stream, mIsSigned);
    else if (dataID == "SRGB")
        SerializationUtils::deserializeData(stream, mIsSRGB);
    else if (dataID == "Compressed")
        SerializationUtils::deserializeData(stream, mIsCompressed);
}

void TextureNode::onDeserialize() {
    if (mIsScreenLocked)
        RenderConfig::addResizeCallable(*this);
    updateTexture();
}

void TextureNode::drawContents() {
    bool changed = false;
    changed |= ImUtils::cycleButton(generateNodeLabelID("TextureType"), (size_t&)mTextureType, (size_t)TextureType::Max,
        [](size_t index) { return getTextureTypeLabel((TextureType)index); });
    if (drawTextureSettings() || drawTextureFormat() || changed)
        updateTexture();

    std::string message;
    ImVec4 colour;
    getTextureStatus(mTexture->getState(), message, colour);
    drawMessage(getInternalFormatLabel(mInternalFormat), ImVec4(1, 0, 1, 1));
    drawMessage(message, colour);

    if (mTexture->getState() == Texture::ErrorState::VALID)
        ImUtils::image(*mTexture, generateNodeLabelID("Preview"), mShowPreview);
}

bool TextureNode::drawTextureSettings() {
    if (!ImUtils::beginHeader("Parameters", generateNodeLabelID("Parameters"), mShowParameters))
        return false;

    bool updateSettings = false;

    ImGui::Text("Bounds:");
    if (ImUtils::button(mIsScreenLocked ? "Unlock from Screen" : "Lock to Screen", generateNodeLabelID("ScreenLock"))) {
        mIsScreenLocked = !mIsScreenLocked;
        if (mIsScreenLocked)
            RenderConfig::addResizeCallable(*this);
        else
            RenderConfig::removeResizeCallable(*this);
        updateSettings = true;
    }
    ImGui::BeginDisabled(mIsScreenLocked);
    updateSettings |= ImUtils::inputIntN(&mTexBounds[0], 2, generateNodeLabelID("TextureBounds"), 1, 4096);
    ImGui::EndDisabled();

    ImGui::Text("Min-Filter");
    updateSettings |= ImUtils::cycleButton(generateNodeLabelID("MinFilter"), (size_t&)mMinFilter, (size_t)Texture::MinFilter::Max,
                             [](size_t index) { return getMinFilterLabel((Texture::MinFilter)index); });

    ImGui::Text("Mag-Filter");
    updateSettings |= ImUtils::cycleButton(generateNodeLabelID("MagFilter"), (size_t&)mMagFilter, (size_t)Texture::MagFilter::Max,
                             [](size_t index) { return getMagFilterLabel((Texture::MagFilter)index); });

    ImGui::Text("Edge-Wrap Mode");
    updateSettings |= ImUtils::cycleButton(generateNodeLabelID("EdgeWrap"), (size_t&)mEdgeWrap, (size_t)Texture::EdgeWrap::Max,
                             [](size_t index) { return getEdgeWrapLabel((Texture::EdgeWrap)index); });

    ImGui::Text("Compare Mode");
    updateSettings |= ImUtils::cycleButton(generateNodeLabelID("CompareMode"), (size_t&)mCompareMode, (size_t)Texture::CompareMode::Max,
                             [](size_t index) { return getCompareModeLabel((Texture::CompareMode)index); });

    ImUtils::endHeader();

    return updateSettings;
}

bool TextureNode::drawTextureFormat() {
    if (mTextureType != TextureType::Colour || !ImUtils::beginHeader("Internal Format", generateNodeLabelID("InternalFormat"), mShowInternalFormat))
        return false;

    bool updateFormat = false;

    ImGui::Text("Channels");
    ImUtils::rangeButtonLabel({"R", "G", "B", "A"});
    updateFormat |= ImUtils::rangeButton(generateNodeLabelID("NumChannels"), mNumChannels, 4);

    ImGui::Text("Type");
    updateFormat |= ImUtils::cycleButton(generateNodeLabelID("DataType"), (size_t&)mDataType, (size_t)DataType::Max,
                                         [](size_t index) { return getDataTypeLabel((DataType)index); });

    ImGui::Text("Precision");
    updateFormat |= ImUtils::cycleButton(generateNodeLabelID("Precision"), (size_t&)mPrecision, (size_t)Precision::Max,
                                         [](size_t index) { return getPrecisionLabel((Precision)index); });

    if (ImUtils::button(mIsSigned ? "Signed" : "Unsigned", generateNodeLabelID("Sign"))) {
        updateFormat = true;
        mIsSigned = !mIsSigned;
    }
    if (ImUtils::button(mIsSRGB ? "sRGB On" : "sRGB Off", generateNodeLabelID("SRGB"))) {
        updateFormat = true;
        mIsSRGB = !mIsSRGB;
    }
    if (ImUtils::button(mIsCompressed ? "Compressed" : "Uncompressed", generateNodeLabelID("Compressed"))) {
        updateFormat = true;
        mIsCompressed = !mIsCompressed;
    }

    ImUtils::endHeader();
    return updateFormat;
}

void TextureNode::updateTexture() {
    switch (mTextureType) {
        case TextureType::Colour:
            mInternalFormat = parseInternalFormat(mNumChannels, mDataType, mPrecision, mIsSigned, mIsSRGB, mIsCompressed);
            break;
        case TextureType::Depth:
            mInternalFormat = Texture::InternalFormat::DepthComponent;
            break;
        case TextureType::Stencil:
            mInternalFormat = Texture::InternalFormat::DepthStencil;
            return;
    }
    if (mIsScreenLocked)
        mTexBounds = RenderConfig::getDefaultViewportBounds();
    mTexture->bind();
    mTexture->setInternalFormat(mInternalFormat);
    mTexture->setFilters(mMinFilter, mMagFilter);
    mTexture->setEdgeWrap(mEdgeWrap);
    mTexture->setCompareMode(mCompareMode);
    mTexture->resize(mTexBounds.x, mTexBounds.y);

    mTextureOut.valueUpdated();
}
