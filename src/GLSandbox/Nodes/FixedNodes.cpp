#include "FixedNodes.h"

#include "RenderPassNode.h"

#include <imgui.h>

EntryNode::EntryNode(IPipelineHandler& pipelineHandler) : Node("Entry"), mPipelineHandler(pipelineHandler) {
    addPort(mExecutionOut);
}

std::vector<std::pair<std::string, std::string>> EntryNode::generateSerializedData() const {
    return {};
}

void EntryNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void EntryNode::drawContents() {
    const std::string UPDATE_LABEL = std::string("Update##Node_").append(std::to_string(getID()));
    if (ImGui::Button(UPDATE_LABEL.c_str()) && validatePipeline())
        updatePipeline();
    if (!mMessage.empty())
        ImGui::TextColored(getMessageColour(mMessageType), "%s", mMessage.c_str());
}

bool EntryNode::validatePipeline() {
    if (!mExecutionOut.isLinked()) {
        mMessage = "Not linked to a render pass";
        mMessageType = MessageType::Error;
        return false;
    }
    const RenderPassNode* current = &dynamic_cast<const RenderPassNode&>(mExecutionOut.getLinkedParent());
    while (current) {
        RenderPassNode::ValidationState state = current->validate();
        switch (state) {
            case RenderPassNode::ValidationState::NoMesh:
                mMessage = "Missing mesh";
                mMessageType = MessageType::Error;
                return false;
            case RenderPassNode::ValidationState::NoShader:
                mMessage = "Missing shader";
                mMessageType = MessageType::Error;
                return false;
            case RenderPassNode::ValidationState::InvalidMesh:
                mMessage = "Invalid mesh";
                mMessageType = MessageType::Error;
                return false;
            case RenderPassNode::ValidationState::InvalidShader:
                mMessage = "Invalid shader";
                mMessageType = MessageType::Error;
                return false;
            default:
            case RenderPassNode::ValidationState::Invalid:
                mMessage = "Invalid state";
                mMessageType = MessageType::Error;
                return false;
            case RenderPassNode::ValidationState::Valid:
                break;
        }

        const RenderPassNode* next = current->getNextPass();
        if (next == current) {
            mMessage = "Infinite loop detected";
            return false;
        }
        current = next;
    }
    mMessage = "Uploaded";
    mMessageType = MessageType::Confirmation;
    return true;
}

void EntryNode::updatePipeline() {
    mPipelineHandler.clearPipeline();

    const RenderPassNode* current = &dynamic_cast<const RenderPassNode&>(mExecutionOut.getLinkedParent());
    while (current) {
        mPipelineHandler.appendPipeline(current->generateCallback());

        current = current->getNextPass();
    }
}

ExitNode::ExitNode() : Node("Exit") {
    addPort(mExecutionIn);
}

std::vector<std::pair<std::string, std::string>> ExitNode::generateSerializedData() const {
    return {};
}

void ExitNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void ExitNode::drawContents() {

}
