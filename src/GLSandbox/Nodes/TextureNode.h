#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Texture.h"
#include "../NodeClassifications.h"

#include <memory>

class TextureNode final : public Node {
public:
    enum class DataType : size_t {
        Clamp = 0,
        Int,
        Float,

        Max,
    };
    enum class Precision : size_t {
        _8 = 0,
        _16,
        _32,

        Max,
    };
    enum class TextureType : std::size_t {
        Colour = 0,
        Depth,
        Stencil,

        Max,
    };

    TextureNode();
    ~TextureNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Texture;
    }

    [[nodiscard]] bool isValid() const;
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final;

    void drawContents() override;
private:
    bool drawTextureSettings();
    bool drawTextureFormat();

    void updateTexture();

    std::unique_ptr<Texture> mTexture;
    Port<Texture*> mTextureOut = Port<Texture*>(*this, IPort::Direction::Out, "TextureOut", "Texture", [&]() { return mTexture.get(); });

    glm::ivec2 mTexBounds = glm::ivec2(512, 512);

    TextureType mTextureType = TextureType::Colour;

    Texture::MagFilter   mMagFilter   = Texture::MagFilter::Linear;
    Texture::MinFilter   mMinFilter   = Texture::MinFilter::Linear;
    Texture::EdgeWrap    mEdgeWrap    = Texture::EdgeWrap::ClampToEdge;
    Texture::CompareMode mCompareMode = Texture::CompareMode::None;

    Texture::InternalFormat mInternalFormat = Texture::InternalFormat::RGBA32F;

    size_t mNumChannels  = 4;
    DataType mDataType   = DataType::Float;
    Precision mPrecision = Precision::_32;
    bool mIsSigned       = false;
    bool mIsSRGB         = false;
    bool mIsCompressed   = false;

    bool mShowParameters = false;
    bool mShowInternalFormat = false;
};

