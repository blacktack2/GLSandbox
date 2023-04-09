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
    const std::vector<Shader::UniformSet> uniforms = mShaderIn.getSingleConnectedValue<Shader*>()->getUniforms();
    for (size_t i = 0; i < uniforms.size(); i++) {
        const Shader::UniformSet& uniformSet = uniforms[i];
        for (size_t j = 0; j < uniformSet.uniforms.size(); j++) {
            const Shader::Uniform& uniform = uniformSet.uniforms[j];
            const std::string name = std::string(uniformSet.name).append("-(").append(uniform.name).append(")");
            const std::string uniqueName = std::string("DynamicIn-").append(std::to_string(i + j * 100));
            std::unique_ptr<IPort> port = std::make_unique<Port<int>>(
                *this, IPort::Direction::In, std::string("DynamicIn").append(std::to_string(i)), name
            );
            addPort(*port);
            mUniformInPorts.push_back(std::move(port));
        }
    }
}

void RenderPassNode::clearUniformPorts() {
    for (const auto& port : mUniformInPorts)
        removePort(*port);
    mUniformInPorts.clear();
}
