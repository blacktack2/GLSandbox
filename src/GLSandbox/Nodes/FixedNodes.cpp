#include "FixedNodes.h"

#include "RenderPassNode.h"

#include <imgui.h>

EntryNode::EntryNode(IPipelineHandler& pipelineHandler) : Node("Entry"), mPipelineHandler(pipelineHandler) {
    addPort(mExecutionOutPort);
}

void EntryNode::serializeContents(std::ofstream& streamOut) const {

}

void EntryNode::deserializeContents(std::ifstream& streamIn) {

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

        current = current->getNextPass();
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

void ExitNode::serializeContents(std::ofstream& streamOut) const {

}

void ExitNode::deserializeContents(std::ifstream& streamIn) {

}

void ExitNode::drawContents() {

}
