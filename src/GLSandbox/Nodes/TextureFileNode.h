#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Texture.h"
#include "../NodeClassifications.h"

#include <memory>

class TextureFileNode final : public Node {
public:
    enum class ValidationState {
        Valid,
        Unloaded,
        InvalidFile,
        InvalidTexture,
    };

    TextureFileNode();
    ~TextureFileNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::TextureFile;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final;

    void drawContents() override;
private:
    void loadFromFile();

    std::unique_ptr<Texture> mTexture = std::make_unique<Texture>();
    Port<Texture*> mTextureOut = Port<Texture*>(*this, IPort::Direction::Out, "TextureOut", "Texture", [&]() { return mTexture.get(); });

    std::string mFilename;

    ValidationState mState = ValidationState::Unloaded;

    bool mShowPreview = false;
};

