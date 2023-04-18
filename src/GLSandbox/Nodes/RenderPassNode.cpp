#include "RenderPassNode.h"

RenderPassNode::RenderPassNode() : Node("Render Pass") {
    addPort(mExecutionIn);
    addPort(mExecutionOut);

    addPort(mFramebufferIn);
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
    drawValidationMessage();
}

bool RenderPassNode::validate() const {
    mValidationState = ValidationState::Unloaded;

    if (mMeshIn.isLinked()) {
        if (mMeshIn.getSingleConnectedValue<Mesh*>()->getState() != Mesh::ErrorState::VALID)
            mValidationState |= ValidationState::InvalidMesh;
    } else {
        mValidationState |= ValidationState::NoMesh;
    }
    if (mShaderIn.isLinked()) {
        if (mShaderIn.getSingleConnectedValue<Shader*>()->getState() != Shader::ErrorState::VALID)
            mValidationState |= ValidationState::InvalidShader;
    } else {
        mValidationState |= ValidationState::NoShader;
    }
    if (mFramebufferIn.isLinked() && !mFramebufferIn.getLinkedParent().validate())
        mValidationState |= ValidationState::InvalidFramebuffer;

    for (const auto& port : mSamplerPorts) {
        if (!port.get().isLinked()) {
            mValidationState |= ValidationState::MissingSampler;
            break;
        }
    }

    return mValidationState == ValidationState::Unloaded;
}

RenderPassNode::pipeline_callback RenderPassNode::generateCallback() const {
    mValidationState = ValidationState::Loaded;

    const Framebuffer* framebuffer = mFramebufferIn.isLinked() ? mFramebufferIn.getSingleConnectedValue<Framebuffer*>() : nullptr;
    const Mesh* mesh = mMeshIn.getSingleConnectedValue<Mesh*>();
    const Shader* shader = mShaderIn.getSingleConnectedValue<Shader*>();

    if (framebuffer) {
        return [framebuffer, mesh, shader]() {
            framebuffer->bind();

            shader->bind();

            mesh->bind();
            mesh->draw();

            Framebuffer::unbind();
        };
    } else {
        return [mesh, shader]() {
            shader->bind();

            mesh->bind();
            mesh->draw();
        };
    }
}

void RenderPassNode::onShaderUpdate() {
    clearUniformPorts();
    if (!mShaderIn.isLinked())
        return;

    Shader* shader = mShaderIn.getSingleConnectedValue<Shader*>();
    shader->bind();
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
                    *this, IPort::Direction::In, uniqueName, name, nullptr, false, true
                );

                Port<port_type>* rawPort = port.get();
                std::string uniformName = uniform.name;
                std::variant<port_type> variant;
                std::visit(VisitOverload{
                    [this, rawPort](Texture* arg2) {
                        mSamplerPorts.push_back(*rawPort);
                    },
                    [rawPort, shader, uniformName](auto arg2) {
                        rawPort->addOnUpdateEvent([rawPort, shader, uniformName]() {
                            if (!rawPort->isLinked())
                                return;
                            std::visit(VisitOverload{
                                [](Texture* arg2) {},
                                [shader, uniformName](auto arg2) {
                                    shader->bind();
                                    shader->setUniform(uniformName, arg2);
                                },
                            }, rawPort->getConnectedValue());
                        });
                    },
                }, variant);
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
    mSamplerPorts.clear();
}

void RenderPassNode::drawValidationMessage() {
    if (mValidationState == ValidationState::Unloaded) {
        drawMessage("Not Loaded", ImVec4(1, 1, 0, 1));
        return;
    } else if (mValidationState == ValidationState::Loaded) {
        drawMessage("Loaded", ImVec4(0, 1, 0, 1));
        return;
    }

    if (!ImUtils::beginHeader("Show Problems", generateNodeLabelID("ProblemHeader")))
        return;

    if (mValidationState & ValidationState::Invalid)
        drawMessage("Unknown Error", ImVec4(1, 0, 0, 1));
    if (mValidationState & ValidationState::NoMesh)
        drawMessage("Missing Mesh", ImVec4(1, 0, 0, 1));
    if (mValidationState & ValidationState::NoShader)
        drawMessage("Missing Shader", ImVec4(1, 0, 0, 1));
    if (mValidationState & ValidationState::InvalidMesh)
        drawMessage("Mesh Invalid", ImVec4(1, 0, 0, 1));
    if (mValidationState & ValidationState::InvalidShader)
        drawMessage("Shader Invalid", ImVec4(1, 0, 0, 1));
    if (mValidationState & ValidationState::InvalidFramebuffer)
        drawMessage("Framebuffer Invalid", ImVec4(1, 0, 0, 1));
    if (mValidationState & ValidationState::MissingSampler)
        drawMessage("Missing Sampler Texture", ImVec4(1, 0, 0, 1));

    ImUtils::endHeader();
}
