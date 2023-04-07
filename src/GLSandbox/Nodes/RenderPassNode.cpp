#include "RenderPassNode.h"

#include <imgui.h>

RenderPassNode::RenderPassNode() : Node("Render Pass") {
    addPort(mExecutionIn);
    addPort(mExecutionOut);

    addPort(mMeshIn);
    addPort(mShaderIn);

    mShaderIn.addOnUpdateEvent([this]() { onShaderUpdate(); });
}

std::vector<std::pair<std::string, std::string>> RenderPassNode::generateSerializedData() const {
    return {};
}

void RenderPassNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void RenderPassNode::drawContents() {
    if (!errorText.empty())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorText.c_str());
}

RenderPassNode::ValidationState RenderPassNode::validate() const {
    if (!mMeshIn.isLinked())
        return ValidationState::NoMesh;
    if (!mShaderIn.isLinked())
        return ValidationState::NoShader;

    const Mesh* mesh = mMeshIn.getSingleConnectedValue<Mesh*>();
    const Shader* shader = mShaderIn.getSingleConnectedValue<Shader*>();
    if (mesh->getState() != Mesh::ErrorState::VALID)
        return ValidationState::InvalidShader;
    if (shader->getState() != Shader::ErrorState::VALID)
        return ValidationState::InvalidMesh;

    return ValidationState::Valid;
}

RenderPassNode::pipeline_callback RenderPassNode::generateCallback() const {
    const Mesh* mesh = mMeshIn.getSingleConnectedValue<Mesh*>();
    const Shader* shader = mShaderIn.getSingleConnectedValue<Shader*>();

    return [mesh, shader]() {
        shader->bind();
        mesh->draw();
    };
}

void RenderPassNode::onShaderUpdate() {
    clearUniformPorts();
    if (!mShaderIn.isLinked())
        return;
    const auto uniforms = mShaderIn.getSingleConnectedValue<Shader*>()->getUniforms();
//    mUniformInPorts.resize(uniforms.size());
//    for (size_t i = 0; i < uniforms.size(); i++) {
//        std::string& portName = mUniformInPorts[i].first;
//        const std::string& uniformName = uniforms[i].first;
//        portName = uniformName;
//
//        auto& portVec = mUniformInPorts[i].second;
//        auto& uniformVec = uniforms[i].second;
//        portVec.resize(uniformVec.size());
//        for (size_t j = 0; j < uniformVec.size(); j++) {
//            portVec[j] = InPort(*this, std::string("DynamicIn").append(std::to_string(j)), uniformVec[j].first, {uniformVec[j].second});
//            addPort(portVec[j]);
//        }
//    }
}

void RenderPassNode::clearUniformPorts() {
//    for (const auto& shaderPass : mUniformInPorts)
//        for (const auto& port : shaderPass.second)
//            removePort(port);
//    mUniformInPorts.clear();
}
