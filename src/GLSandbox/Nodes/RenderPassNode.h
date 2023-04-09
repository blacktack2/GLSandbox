#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Mesh.h"
#include "../../Rendering/Shader.h"

#include "../NodeClassifications.h"

#include <functional>
#include <string>

class RenderPassNode final : public Node {
public:
    typedef std::function<void()> pipeline_callback;
    enum class ValidationState {
        Valid,
        Invalid,
        NoMesh,
        NoShader,
        InvalidMesh,
        InvalidShader,
    };

    RenderPassNode();
    ~RenderPassNode() override = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::RenderPass;
    }

    [[nodiscard]] ValidationState validate() const;
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

    Port<void*> mExecutionIn  = Port<void*>(*this, IPort::Direction::In, "In", "In");
    Port<void*> mExecutionOut = Port<void*>(*this, IPort::Direction::Out, "Out", "Out", [&]() { return (void*)nullptr; });

    Port<Mesh*>   mMeshIn   = Port<Mesh*>(*this, IPort::Direction::In, "MeshIn", "Mesh");
    Port<Shader*> mShaderIn = Port<Shader*>(*this, IPort::Direction::In, "ShaderIn", "Shader");

    std::vector<std::unique_ptr<IPort>> mUniformInPorts{};

    std::string errorText;
};
