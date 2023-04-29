#include "ExecutionNodes.h"

#include "RenderPassNode.h"

#include "../../Rendering/RenderConfig.h"

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

}

std::vector<std::pair<std::string, std::string>> InputNode::generateSerializedData() const {
    return std::vector<std::pair<std::string, std::string>>();
}

void InputNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void InputNode::drawContents() {

}

OutputNode::OutputNode() : Node("Output") {

}

std::vector<std::pair<std::string, std::string>> OutputNode::generateSerializedData() const {
    return std::vector<std::pair<std::string, std::string>>();
}

void OutputNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void OutputNode::drawContents() {

}
