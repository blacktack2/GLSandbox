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

    Shader* shader = mShaderIn.getSingleConnectedValue<Shader*>();
    const std::vector<Shader::UniformSet> uniforms = shader->getUniforms();

    for (size_t i = 0; i < uniforms.size(); i++) {
        const Shader::UniformSet& uniformSet = uniforms[i];
        for (size_t j = 0; j < uniformSet.uniforms.size(); j++) {
            const Shader::Uniform& uniform = uniformSet.uniforms[j];
            const std::string name = std::string(uniformSet.name).append("-(").append(uniform.name).append(")");
            const std::string uniqueName = std::string("DynamicIn-").append(std::to_string(i + j * 100));
            std::visit([this, shader, &uniform, &name, &uniqueName](auto arg) {
                using port_type = std::decay_t<decltype(arg)>;
                std::unique_ptr<Port<port_type>> port = std::make_unique<Port<port_type>>(
                    *this, IPort::Direction::In, uniqueName, name
                );
                Port<port_type>* rawPort = port.get();
                std::string uniformName = uniform.name;
                port->addOnUpdateEvent([rawPort, shader, uniformName]() {
                    if (!rawPort->isLinked())
                        return;
                    std::visit([shader, uniformName](auto arg2) {
                        shader->bind();
                        shader->setUniform(uniformName, arg2);
                    }, rawPort->getConnectedValue());
                });
                addPort(*port);
                mUniformInPorts.push_back(std::move(port));
            }, uniform.value);
        }
    }
}

void RenderPassNode::clearUniformPorts() {
    for (const auto& port : mUniformInPorts)
        removePort(*port);
    mUniformInPorts.clear();
}
