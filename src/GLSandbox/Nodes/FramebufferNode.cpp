#include "FramebufferNode.h"

#include "../../Rendering/Texture.h"

#include "../../Utils/SerializationUtils.h"

#include <climits>

void getFramebufferStatus(Framebuffer::ErrorState state, std::string& message, ImVec4& messageColour) {
    switch (state) {
        case Framebuffer::ErrorState::Invalid:
            message = "Invalid Framebuffer";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::Valid:
            message = "Framebuffer Loaded";
            messageColour = ImVec4(0, 1, 0, 1);
            break;
        case Framebuffer::ErrorState::Undefined:
            message = "Error: Undefined";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::IncompleteAttachment:
            message = "Error: Incomplete Attachment";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::IncompleteMissingAttachment:
            message = "Error: Incomplete/Missing Attachment";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::IncompleteDrawBuffer:
            message = "Error: Incomplete Draw Buffer";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::IncompleteReadBuffer:
            message = "Error: Incomplete Read Buffer";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::IncompleteMultiSample:
            message = "Error: Incomplete Multi-Sample";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
        case Framebuffer::ErrorState::IncompleteLayerTargets:
            message = "Error: Incomplete Layer Targets";
            messageColour = ImVec4(1, 0, 0, 1);
            break;
    }
}

FramebufferNode::FramebufferNode() : Node("Framebuffer") {
    mDepthTexIn        = nullptr;
    mDepthStencilTexIn = nullptr;

    addPort(mFramebufferOut);
}

bool FramebufferNode::validate() const {
    bool isValid = true;

    isValid &= !mEnableDepthBuffer || (mDepthTexIn->isLinked() &&
        std::visit([](const auto& arg) {
            return arg->getInternalFormat() == Texture::InternalFormat::DepthComponent;
        }, mDepthTexIn->getConnectedValue()));
    isValid &= !mEnableDepthStencilBuffer || (mDepthStencilTexIn->isLinked() &&
        std::visit([](const auto& arg) {
            return arg->getInternalFormat() == Texture::InternalFormat::DepthStencil;
        }, mDepthStencilTexIn->getConnectedValue()));

    for (const auto& texPort : mTextureInPorts)
        isValid &= texPort->isLinked();

    return isValid;
}

std::vector<std::pair<std::string, std::string>> FramebufferNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("NumColourBuffers", SerializationUtils::serializeData(mNumColourBuffers));
    data.emplace_back("DepthBuffer", SerializationUtils::serializeData(mEnableDepthBuffer));
    data.emplace_back("DepthStencilBuffer", SerializationUtils::serializeData(mEnableDepthStencilBuffer));
    return data;
}

void FramebufferNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "NumColourBuffers")
        SerializationUtils::deserializeData(stream, mNumColourBuffers);
    else if (dataID == "DepthBuffer")
        SerializationUtils::deserializeData(stream, mEnableDepthBuffer);
    else if (dataID == "DepthStencilBuffer")
        SerializationUtils::deserializeData(stream, mEnableDepthStencilBuffer);
}

void FramebufferNode::onDeserialize() {
    updateTexturePorts();
}

void FramebufferNode::drawContents() {
    drawBufferParameters();

    std::string message;
    ImVec4 colour;
    getFramebufferStatus(mFramebuffer->getState(), message, colour);
    drawMessage(message, colour);
}

void FramebufferNode::drawBufferParameters() {
    bool updatePorts = false;

    ImGui::BeginDisabled(isLocked());

    ImGui::Text("Num. Colour Buffers");
    updatePorts |= ImUtils::inputInt((int*)&mNumColourBuffers, generateNodeLabelID("NumBuffers"), 0, INT_MAX);

    if (ImUtils::button("Depth", generateNodeLabelID("DepthTex"))) {
        updatePorts = true;
        mEnableDepthBuffer = !mEnableDepthBuffer;
    }
    if (ImUtils::button("Depth-Stencil", generateNodeLabelID("DepthStencilTex"))) {
        updatePorts = true;
        mEnableDepthStencilBuffer = !mEnableDepthStencilBuffer;
    }

    ImGui::EndDisabled();

    if (updatePorts)
        updateTexturePorts();
}

void FramebufferNode::updateTexturePorts() {
    unsigned int originalSize = mTextureInPorts.size();

    if (originalSize < mNumColourBuffers) {
        mTextureInPorts.resize(mNumColourBuffers);
        for (unsigned int i = originalSize; i < mNumColourBuffers; i++)
            mTextureInPorts[i] = generateTexPort(std::string("Tex").append(std::to_string(i)));
    } else if (originalSize > mNumColourBuffers) {
        for (unsigned int i = mNumColourBuffers; i < originalSize; i++)
            removePort(*mTextureInPorts[i]);
        mTextureInPorts.resize(mNumColourBuffers);
    }

    if (mEnableDepthBuffer && !mDepthTexIn) {
        mDepthTexIn = generateTexPort("D-Tex");
    } else if (!mEnableDepthBuffer && mDepthTexIn) {
        removePort(*mDepthTexIn);
        mDepthTexIn = nullptr;
    }

    if (mEnableDepthStencilBuffer && !mDepthStencilTexIn) {
        mDepthStencilTexIn = generateTexPort("DS-Tex");
    } else if (!mEnableDepthStencilBuffer && mDepthStencilTexIn) {
        removePort(*mDepthStencilTexIn);
        mDepthStencilTexIn = nullptr;
    }

    updateFramebuffer();
}

void FramebufferNode::updateFramebuffer() {
    mFramebuffer->bind();
    mFramebuffer->reset();

    if (!validate()) {
        mFramebuffer->unbind();
        return;
    }

    if (mEnableDepthBuffer && mDepthTexIn->isLinked())
        mFramebuffer->bindTexture(*std::visit([](auto arg)->Texture* { return arg; }, mDepthTexIn->getConnectedValue()));
    if (mEnableDepthStencilBuffer && mDepthStencilTexIn->isLinked())
        mFramebuffer->bindTexture(*std::visit([](auto arg)->Texture* { return arg; }, mDepthStencilTexIn->getConnectedValue()));
    for (unsigned int i = 0; i < mNumColourBuffers; i++)
        if (mTextureInPorts[i]->isLinked())
            mFramebuffer->bindTexture(*std::visit([](auto arg)->Texture* { return arg; }, mTextureInPorts[i]->getConnectedValue()), i);

    mFramebuffer->drawBuffers();

    mFramebuffer->unbind();
}

std::unique_ptr<Port<Texture*>> FramebufferNode::generateTexPort(const std::string& name) {
    std::unique_ptr<Port<Texture*>> texPort = std::make_unique<Port<Texture*>>(*this, IPort::Direction::In, name, name);
    addPort(*texPort);
    texPort->addOnUpdateEvent([this]() { updateFramebuffer(); });
    return texPort;
}
