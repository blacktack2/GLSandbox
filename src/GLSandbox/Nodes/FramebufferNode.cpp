#include "FramebufferNode.h"

#include "../../Rendering/Texture.h"

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

bool FramebufferNode::isValid() const {
    bool isValid = true;
    isValid &= !mEnableDepthBuffer || mDepthTexIn->isLinked();
    isValid &= !mEnableDepthStencilBuffer || mDepthStencilTexIn->isLinked();
    for (const auto& texPort : mTextureInPorts)
        isValid &= texPort->isLinked();
    return isValid;
}

std::vector<std::pair<std::string, std::string>> FramebufferNode::generateSerializedData() const {
    return {
        {"NumColourBuffers",   std::to_string(mNumColourBuffers)},
        {"DepthBuffer",        std::to_string(mEnableDepthBuffer)},
        {"DepthStencilBuffer", std::to_string(mEnableDepthStencilBuffer)},
    };
}

void FramebufferNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "NumColourBuffers")
        stream >> mNumColourBuffers;
    else if (dataID == "DepthBuffer")
        stream >> mEnableDepthBuffer;
    else if (dataID == "DepthStencilBuffer")
        stream >> mEnableDepthStencilBuffer;
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

    if (updatePorts)
        updateTexturePorts();
}

void FramebufferNode::updateTexturePorts() {
    unsigned int originalSize = mTextureInPorts.size();

    if (originalSize < mNumColourBuffers) {
        mTextureInPorts.resize(mNumColourBuffers);
        for (unsigned int i = originalSize; i < mNumColourBuffers; i++) {
            std::string name = std::string("Tex").append(std::to_string(i));
            mTextureInPorts[i] = std::make_unique<Port<Texture*>>(*this, IPort::Direction::In, name, name);
            addPort(*mTextureInPorts[i]);
            mTextureInPorts[i]->addValidateLinkEvent([](IPort& linkTo) {
                Port<Texture*>* port = dynamic_cast<Port<Texture*>*>(&linkTo);
                return port && std::visit([](auto* arg) {
                    return arg->getInternalFormat() != Texture::InternalFormat::DepthComponent &&
                           arg->getInternalFormat() != Texture::InternalFormat::DepthStencil;
                }, port->getValue());
            });
            mTextureInPorts[i]->addOnUpdateEvent([this]() { updateFramebuffer(); });
        }
    } else if (originalSize > mNumColourBuffers) {
        for (unsigned int i = mNumColourBuffers; i < originalSize; i++)
            removePort(*mTextureInPorts[i]);
        mTextureInPorts.resize(mNumColourBuffers);
    }

    if (mEnableDepthBuffer && !mDepthTexIn) {
        mDepthTexIn = std::make_unique<Port<Texture*>>(*this, IPort::Direction::In, "D-Tex", "DepthTex");
        addPort(*mDepthTexIn);
        mDepthTexIn->addValidateLinkEvent([](IPort& linkTo) {
            Port<Texture*>* port = dynamic_cast<Port<Texture*>*>(&linkTo);
            return port && std::visit([](auto* arg) {
                return arg->getInternalFormat() == Texture::InternalFormat::DepthComponent;
            }, port->getValue());
        });
        mDepthTexIn->addOnUpdateEvent([this]() { updateFramebuffer(); });
    } else if (!mEnableDepthBuffer && mDepthTexIn) {
        removePort(*mDepthTexIn);
        mDepthTexIn = nullptr;
    }

    if (mEnableDepthStencilBuffer && !mDepthStencilTexIn) {
        mDepthStencilTexIn = std::make_unique<Port<Texture*>>(*this, IPort::Direction::In, "DS-Tex", "DepthStencilTex");
        addPort(*mDepthStencilTexIn);
        mDepthStencilTexIn->addValidateLinkEvent([](IPort& linkTo) {
            Port<Texture*>* port = dynamic_cast<Port<Texture*>*>(&linkTo);
            return port && std::visit([](auto* arg) {
                return arg->getInternalFormat() == Texture::InternalFormat::DepthStencil;
            }, port->getValue());
        });
        mDepthStencilTexIn->addOnUpdateEvent([this]() { updateFramebuffer(); });
    } else if (!mEnableDepthStencilBuffer && mDepthStencilTexIn) {
        removePort(*mDepthStencilTexIn);
        mDepthStencilTexIn = nullptr;
    }

    updateFramebuffer();
}

void FramebufferNode::updateFramebuffer() {
    mFramebuffer->bind();
    mFramebuffer->reset();

    if (!isValid()) {
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
