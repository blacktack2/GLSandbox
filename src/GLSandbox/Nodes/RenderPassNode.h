#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Framebuffer.h"
#include "../../Rendering/Mesh.h"
#include "../../Rendering/RenderConfig.h"
#include "../../Rendering/Shader.h"
#include "../../Rendering/Texture.h"

#include "../NodeClassifications.h"

#include <functional>
#include <string>

class RenderPassNode final : public Node {
public:
    typedef std::function<void()> pipeline_callback;
    enum class ValidationState : unsigned int {
        Unloaded           = 0,
        Loaded             = 1 << 0,
        Invalid            = 1 << 1,
        NoMesh             = 1 << 2,
        NoShader           = 1 << 3,
        InvalidMesh        = 1 << 4,
        InvalidShader      = 1 << 5,
        InvalidFramebuffer = 1 << 6,
        MissingSampler     = 1 << 7,
    };

    RenderPassNode();
    ~RenderPassNode() override = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::RenderPass;
    }

    [[nodiscard]] bool validate() const final;

    [[nodiscard]] pipeline_callback generateCallback() const;

    [[nodiscard]] inline const RenderPassNode* getNextPass() const {
        return mExecutionOut.isLinked() ?
            dynamic_cast<const RenderPassNode*>(&mExecutionOut.getLinkedParent()) : nullptr;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    void onShaderUpdate();

    void clearUniformPorts();

    void drawSettings();
    void drawValidationMessage();

    Port<void*> mExecutionIn  = Port<void*>(*this, IPort::Direction::In, "In", "In");
    Port<void*> mExecutionOut = Port<void*>(*this, IPort::Direction::Out, "Out", "Out", [&]() { return (void*)nullptr; });

    Port<Framebuffer*> mFramebufferIn = Port<Framebuffer*>(*this, IPort::Direction::In, "FramebufferIn", "Framebuffer");
    Port<Mesh*>        mMeshIn        = Port<Mesh*>(*this, IPort::Direction::In, "MeshIn", "Mesh");
    Port<Shader*>      mShaderIn      = Port<Shader*>(*this, IPort::Direction::In, "ShaderIn", "Shader");

    std::vector<std::unique_ptr<IPort>> mUniformInPorts{};
    std::vector<std::reference_wrapper<IPort>> mSamplerPorts{};

    glm::vec4 mViewport = glm::vec4(0.0f);

    bool mDoClear = false;
    glm::vec4 mClearColour = glm::vec4(0.0f);

    bool mEnableBlend = false;
    RenderConfig::BlendFuncSrc mBlendFuncSrc = RenderConfig::BlendFuncSrc::One;
    RenderConfig::BlendFuncDst mBlendFuncDst = RenderConfig::BlendFuncDst::Zero;

    glm::bvec4 mColourMask = glm::bvec4(true);

    bool mEnableFaceCulling = true;
    RenderConfig::CullFaceMode mCullFaceMode = RenderConfig::CullFaceMode::Back;

    bool mEnableDepthTest = false;
    RenderConfig::DepthTestFunc mDepthTestFunc = RenderConfig::DepthTestFunc::Less;
    glm::vec2 mDepthTestLimits = glm::vec2(0.0f, 1.0f);

    bool mEnableDepthMask = false;

    mutable ValidationState mValidationState = ValidationState::Unloaded;
};

inline unsigned int operator&(RenderPassNode::ValidationState a, RenderPassNode::ValidationState b) {
    return (unsigned int)a & (unsigned int)b;
}
inline void operator|=(RenderPassNode::ValidationState& a, RenderPassNode::ValidationState b) {
    a = (RenderPassNode::ValidationState)((unsigned int)a | (unsigned int)b);
}
