#include "RenderPassNode.h"

#include "../../Utils/SerializationUtils.h"

std::string getBlendFuncSrcLabel(RenderConfig::BlendFuncSrc srcFactor) {
    switch (srcFactor) {
        default: return "Undefined";
        case RenderConfig::BlendFuncSrc::One                    : return "1";
        case RenderConfig::BlendFuncSrc::SrcColour              : return "Src";
        case RenderConfig::BlendFuncSrc::OneMinusSrcColour      : return "1-Src";
        case RenderConfig::BlendFuncSrc::DstColour              : return "Dst";
        case RenderConfig::BlendFuncSrc::OneMinusDstColour      : return "1-Dst";
        case RenderConfig::BlendFuncSrc::SrcAlpha               : return "SrcA";
        case RenderConfig::BlendFuncSrc::OneMinusSrcAlpha       : return "1-SrcA";
        case RenderConfig::BlendFuncSrc::ConstantColour         : return "Const";
        case RenderConfig::BlendFuncSrc::OneMinusConstantColour : return "1-Const";
        case RenderConfig::BlendFuncSrc::ConstantAlpha          : return "ConstA";
        case RenderConfig::BlendFuncSrc::OneMinusConstantAlpha  : return "1-ConstA";
        case RenderConfig::BlendFuncSrc::SrcAlphaSaturate       : return "SrcA Sat";
    }
}

std::string getBlendFuncDstLabel(RenderConfig::BlendFuncDst dstFactor) {
    switch (dstFactor) {
        default: return "Undefined";
        case RenderConfig::BlendFuncDst::Zero                   : return "0";
        case RenderConfig::BlendFuncDst::One                    : return "1";
        case RenderConfig::BlendFuncDst::SrcColour              : return "Src";
        case RenderConfig::BlendFuncDst::OneMinusSrcColour      : return "1-Src";
        case RenderConfig::BlendFuncDst::DstColour              : return "Dst";
        case RenderConfig::BlendFuncDst::OneMinusDstColour      : return "1-Dst";
        case RenderConfig::BlendFuncDst::SrcAlpha               : return "SrcA";
        case RenderConfig::BlendFuncDst::OneMinusSrcAlpha       : return "1-SrcA";
        case RenderConfig::BlendFuncDst::DstAlpha               : return "DstA";
        case RenderConfig::BlendFuncDst::OneMinusDstAlpha       : return "1-DstA";
        case RenderConfig::BlendFuncDst::ConstantColour         : return "Const";
        case RenderConfig::BlendFuncDst::OneMinusConstantColour : return "1-Const";
        case RenderConfig::BlendFuncDst::ConstantAlpha          : return "ConstA";
        case RenderConfig::BlendFuncDst::OneMinusConstantAlpha  : return "1-ConstA";
    }
}

std::string getCullFaceModeLabel(RenderConfig::CullFaceMode mode) {
    switch (mode) {
        default: return "Undefined";
        case RenderConfig::CullFaceMode::Front        : return "Front";
        case RenderConfig::CullFaceMode::Back         : return "Back";
        case RenderConfig::CullFaceMode::FrontAndBack : return "Both";
    }
}

std::string getDepthTestFuncLabel(RenderConfig::DepthTestFunc func) {
    switch (func) {
        default: return "Undefined";
        case RenderConfig::DepthTestFunc::Never        : return "Never";
        case RenderConfig::DepthTestFunc::Less         : return "Less";
        case RenderConfig::DepthTestFunc::Equal        : return "Equal";
        case RenderConfig::DepthTestFunc::LessEqual    : return "Less-Equal";
        case RenderConfig::DepthTestFunc::Greater      : return "Greater";
        case RenderConfig::DepthTestFunc::NotEqual     : return "Not-Equal";
        case RenderConfig::DepthTestFunc::GreaterEqual : return "Greater-Equal";
        case RenderConfig::DepthTestFunc::Always       : return "Always";
    }
}

RenderPassNode::RenderPassNode() : Node("Render Pass") {
    addPort(mExecutionIn);
    addPort(mExecutionOut);

    addPort(mFramebufferIn);
    addPort(mMeshIn);
    addPort(mShaderIn);

    mShaderIn.addOnUpdateEvent([this]() { onShaderUpdate(); });
}

std::vector<std::pair<std::string, std::string>> RenderPassNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Viewport", SerializationUtils::serializeData(mViewport));
    if (mDoClear)
        data.emplace_back("ClearColour", SerializationUtils::serializeData(mClearColour));
    if (mEnableBlend) {
        data.emplace_back("BlendFuncSrc", SerializationUtils::serializeData((int)mBlendFuncSrc));
        data.emplace_back("BlendFuncDst", SerializationUtils::serializeData((int)mBlendFuncDst));
    }
    data.emplace_back("ColourMask", SerializationUtils::serializeData(mColourMask));
    if (mEnableFaceCulling)
        data.emplace_back("FaceCull", SerializationUtils::serializeData((int)mCullFaceMode));
    if (mEnableDepthTest) {
        data.emplace_back("DepthTestFunc", SerializationUtils::serializeData((int)mDepthTestFunc));
        data.emplace_back("DepthTestLimits", SerializationUtils::serializeData(mDepthTestLimits));
    }
    data.emplace_back("DepthMask", SerializationUtils::serializeData(mEnableDepthMask));
    return data;
}

void RenderPassNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Viewport") {
        SerializationUtils::deserializeData(stream, mViewport);
    } else if (dataID == "ClearColour") {
        mDoClear = true;
        SerializationUtils::deserializeData(stream, mClearColour);
    } else if (dataID == "BlendFuncSrc") {
        mEnableBlend = true;
        SerializationUtils::deserializeData(stream, (int&)mBlendFuncSrc);
    } else if (dataID == "BlendFuncDst") {
        mEnableBlend = true;
        SerializationUtils::deserializeData(stream, (int&)mBlendFuncDst);
    } else if (dataID == "ColourMask") {
        SerializationUtils::deserializeData(stream, mColourMask);
    } else if (dataID == "FaceCull") {
        mEnableFaceCulling = true;
        SerializationUtils::deserializeData(stream, (int&)mCullFaceMode);
    } else if (dataID == "DepthTestFunc") {
        mEnableDepthTest = true;
        SerializationUtils::deserializeData(stream, (int&)mDepthTestFunc);
    } else if (dataID == "DepthTestLimits") {
        mEnableDepthTest = true;
        SerializationUtils::deserializeData(stream, mDepthTestLimits);
    } else if (dataID == "DepthMask") {
        SerializationUtils::deserializeData(stream, mEnableDepthMask);
    }
}

void RenderPassNode::drawContents() {
    drawSettings();
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

    std::vector<std::function<void()>> passCallbacks{};

    bool useFramebuffer = framebuffer != nullptr;

    if (useFramebuffer)
        passCallbacks.emplace_back([framebuffer]() { framebuffer->bind(); });

    passCallbacks.emplace_back([shader]() { shader->bind(); });

    if (useFramebuffer) {
        std::vector<const Texture*> textures{};
        for (const auto& port : mSamplerPorts)
            textures.push_back(dynamic_cast<Port<Texture*>*>(&port.get())->getSingleValue<Texture*>());
        passCallbacks.emplace_back([textures]() {
            for (int i = 0; i < textures.size(); i++)
                textures[i]->bind(i);
        });
    }

    passCallbacks.emplace_back([this]() {
        if (mViewport == glm::vec4(0.0f))
            RenderConfig::setViewport();
        else
            RenderConfig::setViewport(mViewport.x, mViewport.y, mViewport.z, mViewport.w);

        RenderConfig::setBlend(mEnableBlend, mBlendFuncSrc, mBlendFuncDst);
        RenderConfig::setColourMask(mColourMask.r, mColourMask.g, mColourMask.b, mColourMask.a);
        RenderConfig::setCullFace(mEnableFaceCulling, mCullFaceMode);
        RenderConfig::setDepthTest(mEnableDepthTest, mDepthTestFunc, mDepthTestLimits.x, mDepthTestLimits.y);
        RenderConfig::setDepthMask(mEnableDepthMask);

        if (mDoClear) {
            RenderConfig::setClearColour(mClearColour.r, mClearColour.g, mClearColour.b, mClearColour.a);
            RenderConfig::clearBuffers(RenderConfig::ClearBit::Colour | RenderConfig::ClearBit::Depth | RenderConfig::ClearBit::Stencil);
        }
    });

    passCallbacks.emplace_back([mesh]() {
        mesh->bind();
        mesh->draw();
    });

    if (useFramebuffer)
        passCallbacks.emplace_back([]() { Framebuffer::unbind(); });

    return [passCallbacks]() {
        for (const auto& callback : passCallbacks)
            callback();
    };
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

void RenderPassNode::drawSettings() {
    if (!ImUtils::beginHeader("Settings", generateNodeLabelID("SettingsHeader"), mShowSettings))
        return;

    ImGui::Text("Viewport");
    ImUtils::inputFloatN(&mViewport[0], 4, generateNodeLabelID("Viewport"), 0.0f, 4096.0f);

    ImUtils::toggleButton(mDoClear, "Clear Buffers (On)", "Clear Buffers (Off)", generateNodeLabelID("ClearBuffers"));
    if (mDoClear)
        ImUtils::inputFloatN(&mClearColour[0], 4, generateNodeLabelID("ClearColour"), 0.0f, 1.0f);

    ImUtils::toggleButton(mEnableBlend, "Blend (On)", "Blend (Off)", generateNodeLabelID("Blend"));
    if (mEnableBlend) {
        ImGui::Text("Blend Source Function");
        ImUtils::cycleButton(generateNodeLabelID("BlendFuncSrc"), (size_t&)mBlendFuncSrc,
                             (size_t)RenderConfig::BlendFuncSrc::Max,
                             [](size_t index) { return getBlendFuncSrcLabel((RenderConfig::BlendFuncSrc)index); });
        ImGui::Text("Blend Destination Function");
        ImUtils::cycleButton(generateNodeLabelID("BlendFuncDst"), (size_t&)mBlendFuncDst,
                             (size_t)RenderConfig::BlendFuncDst::Max,
                             [](size_t index) { return getBlendFuncDstLabel((RenderConfig::BlendFuncDst)index); });
    }

    ImGui::Text("Colour Mask");
    ImUtils::multiInputLabel("R:", "G:", "B:", "A:");
    ImUtils::inputBoolN(&mColourMask[0], 4, generateNodeLabelID("ColourMask"));

    ImUtils::toggleButton(mEnableFaceCulling, "Face Cull (On)", "Face Cull (Off)", generateNodeLabelID("FaceCull"));
    if (mEnableFaceCulling) {
        ImGui::Text("Face Cull Mode");
        ImUtils::cycleButton(generateNodeLabelID("FaceCullMode"), (size_t&)mCullFaceMode,
                             (size_t)RenderConfig::CullFaceMode::Max,
                             [](size_t index) { return getCullFaceModeLabel((RenderConfig::CullFaceMode)index); });
    }

    ImUtils::toggleButton(mEnableDepthTest, "Depth Test (On)", "Depth Test (Off)", generateNodeLabelID("DepthTest"));
    if (mEnableDepthTest) {
        ImGui::Text("Depth Test Func");
        ImUtils::cycleButton(generateNodeLabelID("DepthTestFunc"), (size_t&)mDepthTestFunc,
                             (size_t)RenderConfig::DepthTestFunc::Max,
                             [](size_t index) { return getDepthTestFuncLabel((RenderConfig::DepthTestFunc)index); });
        ImUtils::multiInputLabel("Near:", "Far:");
        ImUtils::inputFloatN(&mDepthTestLimits[0], 2, generateNodeLabelID("DepthTestLimits"), 0.0f, 1.0f);
    }

    ImUtils::toggleButton(mEnableDepthMask, "Depth Mask (On)", "Depth Mask (Off)", generateNodeLabelID("DepthMask"));

    ImUtils::endHeader();
}

void RenderPassNode::drawValidationMessage() {
    if (mValidationState == ValidationState::Unloaded) {
        drawMessage("Not Loaded", ImVec4(1, 1, 0, 1));
        return;
    } else if (mValidationState == ValidationState::Loaded) {
        drawMessage("Loaded", ImVec4(0, 1, 0, 1));
        return;
    }

    if (!ImUtils::beginHeader("Show Problems", generateNodeLabelID("ProblemHeader"), mShowSettings))
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
