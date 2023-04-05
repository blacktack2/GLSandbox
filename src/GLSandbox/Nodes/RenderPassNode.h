#pragma once
#include "../../NodeEditor/Node.h"

#include "../../Rendering/Mesh.h"
#include "../../Rendering/Shader.h"

#include "../NodeClassifications.h"

#include <functional>
#include <string>

class RenderPassNode final : public Node {
public:
    typedef std::function<void()> pipeline_callback;

    RenderPassNode();
    ~RenderPassNode() override = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::RenderPass;
    }

    [[nodiscard]] bool validate() const;
    [[nodiscard]] pipeline_callback generateCallback() const;

    [[nodiscard]] inline const RenderPassNode* getNextPass() const {
        return mExecutionOutPort.isLinked() ? std::any_cast<RenderPassNode*>(mExecutionOutPort.getLinkValue()) : nullptr;
    }
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() final;
private:
    void onShaderUpdate();

    void clearUniformPorts();

    InPort mExecutionInPort = InPort(*this, "In", {typeid(void*), typeid(RenderPassNode*)});
    OutPort mExecutionOutPort = OutPort(*this, "Out", [&]() { return this; });

    InPort mMeshInPort = InPort(*this, "Mesh", {typeid(Mesh*)});
    InPort mShaderInPort = InPort(*this, "Shader", {typeid(Shader*)});

    std::vector<std::pair<std::string, std::vector<InPort>>> mUniformInPorts{};

    std::string errorText;
};
