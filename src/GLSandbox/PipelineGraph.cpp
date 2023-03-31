#include "PipelineGraph.h"

#include "../NodeEditor/Node.h"
#include "BasicNodes.h"
#include "ExecutableNodes.h"
#include "MathNodes.h"
#include "MeshNode.h"
#include "ShaderNode.h"

#include <imgui.h>

PipelineGraph::PipelineGraph() {
    ShaderNode::findVertexFiles();
    ShaderNode::findFragmentFiles();
    ShaderNode::findTessContFiles();
    ShaderNode::findTessEvalFiles();
    ShaderNode::findGeometryFiles();

    std::unique_ptr<EntryNode> defaultEntry = std::make_unique<EntryNode>();
    std::unique_ptr<RenderPassNode> defaultPass = std::make_unique<RenderPassNode>();
    std::unique_ptr<ExitNode> defaultExit = std::make_unique<ExitNode>();
    dynamic_cast<OutPort&>(defaultEntry->getPortByIndex(0)).link(dynamic_cast<InPort&>(defaultPass->getPortByIndex(0)));
    dynamic_cast<OutPort&>(defaultPass->getPortByIndex(1)).link(dynamic_cast<InPort&>(defaultExit->getPortByIndex(0)));
    defaultEntry->setAbsolutePosition({0, 100});
    defaultPass->setAbsolutePosition({100, 100});
    defaultExit->setAbsolutePosition({300, 100});
    addNode(std::move(defaultEntry));
    addNode(std::move(defaultPass));
    addNode(std::move(defaultExit));
}

void PipelineGraph::drawNodeCreation() {
    if (ImGui::CollapsingHeader("Execution##ExecutionHeader")) {
        static const std::vector<NodeFactory> cEXECUTION_FACTORIES = {
            NodeFactory("Entry", []() { return std::make_unique<EntryNode>(); }),
            NodeFactory("Render Pass", []() { return std::make_unique<RenderPassNode>(); }),
        };
        drawNodeSelectors(cEXECUTION_FACTORIES);
    }
    if (ImGui::CollapsingHeader("Maths##MeshHeader")) {
        static const std::vector<NodeFactory> cMATH_FACTORIES = {
            NodeFactory("Arithmetic", []() { return std::make_unique<ArithmeticNode>(); }),
        };
        drawNodeSelectors(cMATH_FACTORIES);
    }
    if (ImGui::CollapsingHeader("Numerics##NumericsHeader")) {
        static const std::vector<NodeFactory> cNUMERIC_FACTORIES = {
            NodeFactory("Integer", []() { return std::make_unique<IntegerNode>(); }),
            NodeFactory("Float"  , []() { return std::make_unique<FloatNode>();   }),
        };
        drawNodeSelectors(cNUMERIC_FACTORIES);
    }
    if (ImGui::CollapsingHeader("Graphics##GraphicsHeader")) {
        static const std::vector<NodeFactory> cGRAPHICS_FACTORIES = {
            NodeFactory("Mesh"  , []() { return std::make_unique<MeshNode>();  }),
            NodeFactory("Shader", []() { return std::make_unique<ShaderNode>();}),
        };
        drawNodeSelectors(cGRAPHICS_FACTORIES);
    }
}

void PipelineGraph::drawNodeSelectors(const std::vector<NodeFactory>& factories) {
    for (const NodeFactory& nodeFactory : factories)
        if (ImGui::Selectable(nodeFactory.mLabel.c_str()))
            addNode(nodeFactory.mCallback());
}
