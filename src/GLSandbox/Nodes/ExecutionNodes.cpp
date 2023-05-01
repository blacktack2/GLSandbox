#include "ExecutionNodes.h"

#include "RenderPassNode.h"

#include "../../Rendering/Framebuffer.h"
#include "../../Rendering/Mesh.h"
#include "../../Rendering/Shader.h"
#include "../../Rendering/Texture.h"

EntryNode::EntryNode(IPipelineHandler& pipelineHandler) : Node("Entry"), mPipelineHandler(pipelineHandler) {
    addPort(mExecutionOut);
}

std::vector<std::pair<std::string, std::string>> EntryNode::generateSerializedData() const {
    return {};
}

void EntryNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void EntryNode::drawContents() {
    if (!isLocked())
        if (ImUtils::button("Update", generateNodeLabelID("Update")))
            pipelineUpdateEvent();
    if (isLocked())
        if (ImUtils::button("Reset", generateNodeLabelID("Reset")))
            pipelineResetEvent();

    if (!mMessage.empty())
        drawMessage(mMessage, getMessageColour(mMessageType));
}

void EntryNode::pipelineUpdateEvent() {
    if (validatePipeline())
        updatePipeline();
    else
        mPipelineHandler.resetPipeline();
}

void EntryNode::pipelineResetEvent() {
    unlock();
    mPipelineHandler.resetPipeline();
}

bool EntryNode::validatePipeline() const {
    if (!mExecutionOut.isLinked()) {
        mMessage = "Not linked to a render pass";
        mMessageType = MessageType::Error;
        return false;
    }
    const RenderPassNode* current = &dynamic_cast<const RenderPassNode&>(mExecutionOut.getLinkedParent(0));
    bool isValid = true;
    while (current) {
        isValid &= current->validate();

        const RenderPassNode* next = current->getNextPass();
        if (next == current) {
            mMessage = "Infinite loop detected";
            return false;
        }
        current = next;
    }
    if (isValid) {
        mMessage = "Uploaded";
        mMessageType = MessageType::Confirmation;
        return true;
    } else {
        mMessage = "Invalid";
        mMessageType = MessageType::Error;
        return false;
    }
}

void EntryNode::updatePipeline() {
    mPipelineHandler.clearPipeline();

    const RenderPassNode* current = &dynamic_cast<RenderPassNode&>(mExecutionOut.getLinkedParent(0));
    while (current) {
        mPipelineHandler.appendPipeline(current->generateCallback());

        current = current->getNextPass();
    }
    mPipelineHandler.appendPipeline([]() {
        RenderConfig::setViewport();
        RenderConfig::setClearColour();
        RenderConfig::setBlend();
        RenderConfig::setColourMask();
        RenderConfig::setCullFace();
        RenderConfig::setDepthTest();
        RenderConfig::setDepthMask();
    });

    lock();
}

InputNode::InputNode() : Node("Input") {
    addPort(mDefaultIn);

    mValueOut = nullptr;

    mDefaultIn.addOnLinkEvent([this]() {
        std::visit([this](const auto& arg) {
            using port_type = std::decay_t<decltype(arg)>;
            mValueOut = std::make_unique<Port<port_type>>(*this, IPort::Direction::Out, "Out", "Out", [this]() {
                return std::visit(VisitOverload{
                    [](const auto& arg2) { return port_type{}; },
                    [](const port_type& arg2) { return arg2; },
                }, mExternalInput ? *mExternalInput : mDefaultIn.getValue());
            }, false, true);
            addPort(*mValueOut);
        }, mDefaultIn.getValue());
    });
    mDefaultIn.addOnUnlinkEvent([this]() {
        removePort(*mValueOut);
        mValueOut = nullptr;
    });
}

void InputNode::drawInput() {
    ImGui::Text("%s -> ", getName().c_str());

    ImGui::SameLine();

    switch (isValid()) {
        case ValidationState::Valid:
            std::visit(VisitOverload{
                [&](int value) {
                    if (ImUtils::inputIntN(&value, 1, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::ivec2 value) {
                    if (ImUtils::inputIntN(&value[0], 2, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::ivec3 value) {
                    if (ImUtils::inputIntN(&value[0], 3, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::ivec4 value) {
                    if (ImUtils::inputIntN(&value[0], 4, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](const auto& value) { drawMessage("Undefined", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); },
            }, mExternalInput ? *mExternalInput : mDefaultIn.getValue());

            ImGui::SameLine();

            if (ImUtils::button("Reset", generateNodeLabelID("InputReset")))
                mExternalInput = nullptr;

            break;
        default:
        case ValidationState::Unlinked:
            drawMessage("Must be linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            break;
    }
}

std::vector<std::pair<std::string, std::string>> InputNode::generateSerializedData() const {
    return {};
}

void InputNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void InputNode::drawContents() {
    if (!mDefaultIn.isLinked())
        drawMessage("Must have a default value linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    else if (!mValueOut->isLinked())
        drawMessage("Output should be linked to something", ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
}

OutputNode::OutputNode() : Node("Output") {
    addPort(mValueIn);
}

void OutputNode::drawOutput() {
    ImGui::Text("%s -> ", getName().c_str());

    ImGui::SameLine();

    switch (isValid()) {
        case ValidationState::Valid:
            std::visit(VisitOverload{
                [&](const int&        value) { ImGui::Text("%d", value); },
                [&](const glm::ivec2& value) { ImGui::Text("%d, %d", value.x, value.y); },
                [&](const glm::ivec3& value) { ImGui::Text("%d, %d, %d", value.x, value.y, value.z); },
                [&](const glm::ivec4& value) { ImGui::Text("%d, %d, %d, %d", value.x, value.y, value.z, value.w); },
                [&](const auto& value) { drawMessage("Undefined", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); },
            }, mValueIn.getValue());
            break;
        default:
        case ValidationState::Unlinked:
            drawMessage("Must be linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            break;
    }
}

std::vector<std::pair<std::string, std::string>> OutputNode::generateSerializedData() const {
    return {};
}

void OutputNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void OutputNode::drawContents() {
    if (!mValueIn.isLinked())
        drawMessage("Must have a value linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
}
