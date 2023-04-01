#pragma once
#include "../NodeEditor/Node.h"

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

    [[nodiscard]] bool validate();
    [[nodiscard]] pipeline_callback generateCallback();
protected:
    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "In", {&typeid(EntryNode), &typeid(RenderPassNode)});
    OutPort mExecutionOutPort = OutPort(*this, "Out");

    InPort mMeshInPort = InPort(*this, "Mesh", {&typeid(MeshNode)});
    InPort mShaderInPort = InPort(*this, "Shader", {&typeid(ShaderNode)});

    std::string errorText;
};
