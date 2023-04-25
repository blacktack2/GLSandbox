#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Framebuffer.h"
#include "../NodeClassifications.h"

#include <memory>
#include <vector>

class FramebufferNode final : public Node {
public:
    FramebufferNode();
    ~FramebufferNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Framebuffer;
    }

    [[nodiscard]] bool validate() const final;
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final;

    void drawContents() override;
private:
    void drawBufferParameters();

    void updateTexturePorts();
    void updateFramebuffer();

    std::unique_ptr<Port<Texture*>> generateTexPort(const std::string& name);

    std::unique_ptr<Framebuffer> mFramebuffer = std::make_unique<Framebuffer>();
    Port<Framebuffer*> mFramebufferOut = Port<Framebuffer*>(*this, IPort::Direction::Out, "FramebufferOut", "Framebuffer", [&]() { return mFramebuffer.get(); });

    std::vector<std::unique_ptr<Port<Texture*>>> mTextureInPorts{};
    std::unique_ptr<Port<Texture*>> mDepthTexIn;
    std::unique_ptr<Port<Texture*>> mDepthStencilTexIn;

    unsigned int mNumColourBuffers = 0;
    bool mEnableDepthBuffer = false;
    bool mEnableDepthStencilBuffer = false;
};

