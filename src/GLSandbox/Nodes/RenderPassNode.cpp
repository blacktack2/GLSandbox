#include "RenderPassNode.h"

#include <imgui.h>

RenderPassNode::RenderPassNode() : Node("Render Pass") {
    addPort(mExecutionInPort);
    addPort(mExecutionOutPort);
    addPort(mMeshInPort);
    addPort(mShaderInPort);
}

void RenderPassNode::serializeContents(std::ofstream& streamOut) const {

}

void RenderPassNode::deserializeContents(std::ifstream& streamIn) {

}

void RenderPassNode::drawContents() {
    if (!errorText.empty())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorText.c_str());
}

bool RenderPassNode::validate() const {
    if (!mMeshInPort.isLinked() || !mShaderInPort.isLinked())
        return false;

    const Mesh* mesh = std::any_cast<Mesh*>(mMeshInPort.getLinkValue());
    const Shader* shader = std::any_cast<Shader*>(mShaderInPort.getLinkValue());
    if (mesh->getState() != Mesh::ErrorState::VALID || shader->getState() != Shader::ErrorState::VALID)
        return false;

    return true;
}

RenderPassNode::pipeline_callback RenderPassNode::generateCallback() const {
    const Mesh*   mesh   = std::any_cast<Mesh*>(mMeshInPort.getLinkValue());
    const Shader* shader = std::any_cast<Shader*>(mShaderInPort.getLinkValue());

    return [mesh, shader]() {
        shader->bind();
        mesh->draw();
    };
}
