#include "PipelineGraph.h"

#include "../NodeEditor/Node.h"
#include "Nodes/BasicNodes.h"
#include "Nodes/FixedNodes.h"
#include "Nodes/MathNodes.h"
#include "Nodes/MeshNode.h"
#include "Nodes/RenderPassNode.h"
#include "Nodes/ShaderNode.h"

#include <imgui.h>

PipelineGraph::PipelineGraph(IPipelineHandler& pipelineHandler) : mPipelineHandler(pipelineHandler) {

}

void PipelineGraph::initializeDefault() {
    clearNodes();

    std::unique_ptr<EntryNode> defaultEntry = std::make_unique<EntryNode>(mPipelineHandler);
    std::unique_ptr<RenderPassNode> defaultPass = std::make_unique<RenderPassNode>();
    std::unique_ptr<ExitNode> defaultExit = std::make_unique<ExitNode>();

    defaultEntry->getPortByIndex(0).link(defaultPass->getPortByIndex(0));
    defaultPass->getPortByIndex(1).link(defaultExit->getPortByIndex(0));

    defaultEntry->setAbsolutePosition({0, 100});
    defaultPass->setAbsolutePosition({100, 100});
    defaultExit->setAbsolutePosition({300, 100});

    addNode(std::move(defaultEntry));
    addNode(std::move(defaultPass));
    addNode(std::move(defaultExit));
}

std::unique_ptr<Node> PipelineGraph::deserializeNodeType(unsigned int nodeType) {
    return generateNode((NodeType)nodeType);
}

void PipelineGraph::drawNodeCreation() {
    for (int i = 0; i < (int)NodeGroup::Max; i++) {
        NodeGroup group = (NodeGroup)i;
        const std::string HEADER_LABEL = getGroupName(group).append("##GraphHeader");
        if (!ImGui::CollapsingHeader(HEADER_LABEL.c_str()))
            continue;

        for (const auto& nodeType : gNODE_GROUPS.find(group)->second) {
            const std::string NODE_LABEL = getNodeName(nodeType).append("##GraphButton");
            if (ImGui::Selectable(NODE_LABEL.c_str()))
                addNode(generateNode(nodeType));
        }
    }
}

std::unique_ptr<Node> PipelineGraph::generateNode(NodeType type) const {
    switch (type) {
        case NodeType::Entry      : return std::make_unique<EntryNode>(mPipelineHandler);
        case NodeType::Exit       : return std::make_unique<ExitNode>();

        case NodeType::RenderPass : return std::make_unique<RenderPassNode>();

        case NodeType::Arithmetic : return std::make_unique<ArithmeticNode>();

        case NodeType::Integer    : return std::make_unique<IntegerNode>();
        case NodeType::IVec2      : // Not implemented
        case NodeType::IVec3      :
        case NodeType::IVec4      : return nullptr;

        case NodeType::Float      : return std::make_unique<FloatNode>();
        case NodeType::Vec2       : return std::make_unique<Vec2Node>();
        case NodeType::Vec3       : return std::make_unique<Vec3Node>();
        case NodeType::Vec4       : return std::make_unique<Vec4Node>();

        case NodeType::Mesh       : return std::make_unique<MeshNode>();
        case NodeType::Shader     : return std::make_unique<ShaderNode>();

        default:
            return nullptr;
    }
}

std::string PipelineGraph::getNodeName(NodeType type) {
    switch (type) {
        case NodeType::Entry: return "Entry";
        case NodeType::Exit : return "Exit";
        case NodeType::RenderPass: return "Render Pass";

        case NodeType::Arithmetic: return "Arithmetic";

        case NodeType::Integer: return "Integer";
        case NodeType::Float  : return "Float";
        case NodeType::Vec2   : return "Vec2";
        case NodeType::Vec3   : return "Vec3";
        case NodeType::Vec4   : return "Vec4";

        case NodeType::Mesh  : return "Mesh";
        case NodeType::Shader: return "Shader";

        default: return "Undefined";
    }
}

std::string PipelineGraph::getGroupName(NodeGroup group) {
    switch (group) {
        case NodeGroup::Execution: return "Execution";
        case NodeGroup::Maths    : return "Maths";
        case NodeGroup::Numerics : return "Numerics";
        case NodeGroup::Graphics : return "Graphics";

        default: return "Undefined";
    }
}
