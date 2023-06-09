#include "PipelineGraph.h"

#include "Nodes/BasicNodes.h"
#include "Nodes/ExtendedNodes.h"
#include "Nodes/ExecutionNodes.h"
#include "Nodes/FlowControlNodes.h"
#include "Nodes/FramebufferNode.h"
#include "Nodes/MathNodes.h"
#include "Nodes/MeshNode.h"
#include "Nodes/RenderPassNode.h"
#include "Nodes/ShaderNode.h"
#include "Nodes/SystemNodes.h"
#include "Nodes/TextureFileNode.h"
#include "Nodes/TextureNode.h"

#include <imgui.h>

PipelineGraph::PipelineGraph(IPipelineHandler& pipelineHandler) : mPipelineHandler(pipelineHandler) {

}

void PipelineGraph::onDefaultInitialize() {
    clearNodes();

    std::unique_ptr<EntryNode> defaultEntry = std::make_unique<EntryNode>(mPipelineHandler);
    std::unique_ptr<RenderPassNode> defaultPass = std::make_unique<RenderPassNode>();

    defaultEntry->getPortByIndex(0).link(defaultPass->getPortByIndex(0));

    defaultEntry->setAbsolutePosition({0, 100});
    defaultPass->setAbsolutePosition({100, 100});

    addNode(std::move(defaultEntry));
    addNode(std::move(defaultPass));
}

std::unique_ptr<Node> PipelineGraph::deserializeNodeType(const std::string& nodeType) {
    return generateNode(gNODE_SERIALS.find(nodeType)->second);
}

std::string PipelineGraph::getNodeSerialName(const Node& node) const {
    return getNodeData((NodeType)node.getTypeID()).serializedName;
}

void PipelineGraph::drawNodeCreation() {
    for (int i = 0; i < (int)NodeGroup::Max; i++) {
        NodeGroup group = (NodeGroup)i;
        const std::string HEADER_LABEL = getGroupName(group).append("##GraphHeader");
        if (!ImGui::CollapsingHeader(HEADER_LABEL.c_str()))
            continue;

        for (const auto& nodeType : gNODE_GROUPS.find(group)->second) {
            const NodeData& nodeData = getNodeData(nodeType);
            const std::string NODE_LABEL = std::string(nodeData.displayName).append("##GraphButton");
            if (ImGui::Selectable(NODE_LABEL.c_str()))
                addNode(generateNode(nodeType));

            if (ImGui::IsItemHovered() && !nodeData.tooltip.empty()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(nodeData.tooltip.c_str());
                ImGui::EndTooltip();
            }
        }
    }
}

void PipelineGraph::drawInputPanelContents() {
    for (InputNode& node : mInputNodes)
            node.drawInput();
}

void PipelineGraph::drawOutputPanelContents() {
    for (OutputNode& node : mOutputNodes)
        node.drawOutput();
}

void PipelineGraph::onClear() {
    mInputNodes.clear();
    mOutputNodes.clear();
}

void PipelineGraph::onNodeAdd(Node& node) {
    if (InputNode* inNode = dynamic_cast<InputNode*>(&node))
        mInputNodes.emplace_back(*inNode);
    else if (OutputNode* outNode = dynamic_cast<OutputNode*>(&node))
        mOutputNodes.emplace_back(*outNode);
}

void PipelineGraph::onNodeDelete(Node& node) {
    mInputNodes.erase(std::remove_if(mInputNodes.begin(), mInputNodes.end(), [&node](const auto& inputNode) {
        return &node == &inputNode.get();
    }), mInputNodes.end());
    mOutputNodes.erase(std::remove_if(mOutputNodes.begin(), mOutputNodes.end(), [&node](const auto& outputNode) {
        return &node == &outputNode.get();
    }), mOutputNodes.end());
}

std::unique_ptr<Node> PipelineGraph::generateNode(NodeType type) const {
    switch (type) {
        case NodeType::Entry       : return std::make_unique<EntryNode>(mPipelineHandler);
        case NodeType::Input       : return std::make_unique<InputNode>();
        case NodeType::Output      : return std::make_unique<OutputNode>();
        case NodeType::Graph       : return std::make_unique<SubGraphNode>((Graph&)*this, mPipelineHandler);

        case NodeType::Loop        : return std::make_unique<LoopNode>();

        case NodeType::RenderPass  : return std::make_unique<RenderPassNode>();

        case NodeType::Arithmetic  : return std::make_unique<ArithmeticNode>();

        case NodeType::Integer     : return std::make_unique<IntegerNode>();
        case NodeType::IVec2       : return std::make_unique<IVec2Node>();
        case NodeType::IVec3       : return std::make_unique<IVec3Node>();
        case NodeType::IVec4       : return std::make_unique<IVec4Node>();

        case NodeType::Float       : return std::make_unique<FloatNode>();
        case NodeType::Vec2        : return std::make_unique<Vec2Node>();
        case NodeType::Vec3        : return std::make_unique<Vec3Node>();
        case NodeType::Vec4        : return std::make_unique<Vec4Node>();

        case NodeType::Mat2        : return std::make_unique<Mat2Node>();
        case NodeType::Mat3        : return std::make_unique<Mat3Node>();
        case NodeType::Mat4        : return std::make_unique<Mat4Node>();

        case NodeType::Mesh        : return std::make_unique<MeshNode>();
        case NodeType::Shader      : return std::make_unique<ShaderNode>();
        case NodeType::Texture     : return std::make_unique<TextureNode>();
        case NodeType::Framebuffer : return std::make_unique<FramebufferNode>();
        case NodeType::TextureFile : return std::make_unique<TextureFileNode>();

        case NodeType::UV          : return std::make_unique<UVNode>();

        case NodeType::Colour      : return std::make_unique<ColourNode>();
        case NodeType::Direction   : return std::make_unique<DirectionNode>();

        case NodeType::ModelMatrix : return std::make_unique<ModelMatrixNode>();
        case NodeType::ViewMatrix  : return std::make_unique<ViewMatrixNode>();
        case NodeType::ProjMatrix  : return std::make_unique<ProjMatrixNode>();

        case NodeType::ScreenWidth : return std::make_unique<ScreenWidthNode>();

        default: return nullptr;
    }
}

std::string PipelineGraph::getGroupName(NodeGroup group) {
    switch (group) {
        case NodeGroup::Execution: return "Execution";
        case NodeGroup::Maths    : return "Maths";
        case NodeGroup::Numerics : return "Numerics";
        case NodeGroup::Data     : return "Data";
        case NodeGroup::Graphics : return "Graphics";
        case NodeGroup::System   : return "System";

        default: return "Undefined";
    }
}
