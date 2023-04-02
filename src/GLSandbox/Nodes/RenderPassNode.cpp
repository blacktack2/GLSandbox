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
    const MeshNode* meshNode = dynamic_cast<const MeshNode*>(mMeshInPort.getInput());
    if (!meshNode)
        return false;
    const ShaderNode* shaderNode = dynamic_cast<const ShaderNode*>(mShaderInPort.getInput());
    if (!shaderNode)
        return false;
    if (!meshNode->isValid())
        return false;
    return true;
}

RenderPassNode::pipeline_callback RenderPassNode::generateCallback() const {
    const Mesh*   mesh   = &dynamic_cast<const MeshNode*>(mMeshInPort.getInput())->getMesh();
    const Shader* shader = &dynamic_cast<const ShaderNode*>(mShaderInPort.getInput())->getShader();

    return [mesh, shader]() {
        shader->bind();
        mesh->draw();
    };
}
