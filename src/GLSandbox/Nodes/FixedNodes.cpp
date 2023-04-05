#include "FixedNodes.h"

#include "RenderPassNode.h"

#include <imgui.h>

EntryNode::EntryNode(IPipelineHandler& pipelineHandler) : Node("Entry"), mPipelineHandler(pipelineHandler) {
    addPort(mExecutionOutPort);
}

std::map<std::string, std::string> EntryNode::generateSerializedData() const {
    return std::map<std::string, std::string>();
}

void EntryNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void EntryNode::drawContents() {
    const std::string UPDATE_LABEL = std::string("Update##Node_").append(std::to_string(getID()));
    if (ImGui::Button(UPDATE_LABEL.c_str()) && validatePipeline())
        updatePipeline();
}

bool EntryNode::validatePipeline() {
    if (!mExecutionOutPort.isLinked())
        return false;
    const RenderPassNode* current = &dynamic_cast<const RenderPassNode&>(mExecutionOutPort.getLinkParent());
    while (current) {
        if (!current->validate())
            return false;

        const RenderPassNode* next = current->getNextPass();
        if (next == current)
            return false;
    }
    return true;
}

void EntryNode::updatePipeline() {
    mPipelineHandler.clearPipeline();

    const RenderPassNode* current = &dynamic_cast<const RenderPassNode&>(mExecutionOutPort.getLinkParent());
    while (current) {
        mPipelineHandler.appendPipeline(current->generateCallback());

        current = current->getNextPass();
    }
}

ExitNode::ExitNode() : Node("Exit") {
    addPort(mExecutionInPort);
}

std::map<std::string, std::string> ExitNode::generateSerializedData() const {
    return std::map<std::string, std::string>();
}

void ExitNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void ExitNode::drawContents() {

}
