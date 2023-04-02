#pragma once
#include "../../NodeEditor/Node.h"

#include "../NodeClassifications.h"
#include "FixedNodes.h"
#include "MeshNode.h"
#include "ShaderNode.h"

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
        return dynamic_cast<const RenderPassNode*>(mExecutionOutPort.getLink());
    }
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "In", {&typeid(EntryNode), &typeid(RenderPassNode)});
    OutPort mExecutionOutPort = OutPort(*this, "Out");

    InPort mMeshInPort = InPort(*this, "Mesh", {&typeid(MeshNode)});
    InPort mShaderInPort = InPort(*this, "Shader", {&typeid(ShaderNode)});

    std::string errorText;
};
