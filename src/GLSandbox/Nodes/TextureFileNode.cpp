#include "TextureFileNode.h"

#include "../Assets.h"

#include "../../Utils/SerializationUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void getTextureStatus(TextureFileNode::ValidationState state, std::string& message, ImVec4& messageColour) {
    switch (state) {
        case TextureFileNode::ValidationState::Valid:
            message = "Texture Loaded";
            messageColour = ImVec4(0, 1, 0, 1);
            break;
        default:
        case TextureFileNode::ValidationState::Unloaded:
            message = "Texture Not Loaded";
            messageColour = ImVec4(1, 1, 0, 1);
            break;
        case TextureFileNode::ValidationState::InvalidFile:
            message = "Invalid File";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case TextureFileNode::ValidationState::InvalidTexture:
            message = "Invalid Texture";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
    }
}

TextureFileNode::TextureFileNode() : Node("Texture") {
    addPort(mTextureOut);
}

std::vector<std::pair<std::string, std::string>> TextureFileNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("File", mFilepath.generic_string());
    return data;
}

void TextureFileNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "File")
        mFilepath = SerializationUtils::readLine(stream);
}

void TextureFileNode::onDeserialize() {
    if (!mFilepath.empty())
        loadFromFile();
}

void TextureFileNode::drawContents() {
    ImGui::Text("File");
    if (ImUtils::fileChooseDialog(mFilepath, getTextureAssetDirectory(), generateNodeLabelID("FileChoose"),
                                  getValidTextureFileExtensions()))
        loadFromFile();

    std::string message;
    ImVec4 colour;
    getTextureStatus(mState, message, colour);
    drawMessage(message, colour);

    if (mTexture->getState() == Texture::ErrorState::VALID)
        ImUtils::image(*mTexture, generateNodeLabelID("Preview"), mShowPreview);
}

void TextureFileNode::loadFromFile() {
    int width, height, channels;
    unsigned char* data = stbi_load(mFilepath.generic_string().c_str(), &width, &height, &channels, 0);
    if (!data) {
        mState = ValidationState::InvalidFile;
        stbi_image_free(data);
        return;
    }
    Texture::InternalFormat internalFormat;
    Texture::DataFormat dataFormat;
    switch (channels) {
        default:
        case 1:
            internalFormat = Texture::InternalFormat::R32F;
            dataFormat = Texture::DataFormat::R;
            break;
        case 2:
            internalFormat = Texture::InternalFormat::RG32F;
            dataFormat = Texture::DataFormat::RG;
            break;
        case 3:
            internalFormat = Texture::InternalFormat::RGB32F;
            dataFormat = Texture::DataFormat::RGB;
            break;
        case 4:
            internalFormat = Texture::InternalFormat::RGBA32F;
            dataFormat = Texture::DataFormat::RGBA;
            break;
    }

    mTexture->bind();
    mTexture->setInternalFormat(internalFormat);
    mTexture->resize(width, height);
    mTexture->uploadData(data, dataFormat, Texture::DataType::UnsignedByte);

    stbi_image_free(data);

    mState = mTexture->getState() == Texture::ErrorState::VALID ? ValidationState::Valid : ValidationState::InvalidTexture;
}
