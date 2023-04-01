#include "FixedNodes.h"

#include "RenderPassNode.h"

#include <imgui.h>

EntryNode::EntryNode(IPipelineHandler& pipelineHandler) : Node("Entry"), mPipelineHandler(pipelineHandler) {
    addPort(mExecutionOutPort);
}

void EntryNode::drawContents() {
    const std::string UPDATE_LABEL = std::string("Update##Node_").append(std::to_string(getID()));
    if (ImGui::Button(UPDATE_LABEL.c_str()) && validatePipeline())
        updatePipeline();
}

bool EntryNode::validatePipeline() {
    const RenderPassNode* current = dynamic_cast<const RenderPassNode*>(mExecutionOutPort.getLink());
    while (current) {
        if (!current->validate())
            return false;

        current = dynamic_cast<const RenderPassNode*>(current->getNextPass());
    }
    return true;
}

void EntryNode::updatePipeline() {
    mPipelineHandler.clearPipeline();

    const RenderPassNode* current = dynamic_cast<const RenderPassNode*>(mExecutionOutPort.getLink());
    while (current) {
        mPipelineHandler.appendPipeline(current->generateCallback());

        current = dynamic_cast<const RenderPassNode*>(current->getNextPass());
    }
}

ExitNode::ExitNode() : Node("Exit") {
    addPort(mExecutionInPort);
}

void ExitNode::drawContents() {

}
