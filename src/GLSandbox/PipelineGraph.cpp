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
    mNodeFactories = {
        {NodeType::Entry,     [&]() { return std::make_unique<EntryNode>(mPipelineHandler); }},
        {NodeType::Exit,       []() { return std::make_unique<ExitNode>();       }},
        {NodeType::RenderPass, []() { return std::make_unique<RenderPassNode>(); }},
        {NodeType::Arithmetic, []() { return std::make_unique<ArithmeticNode>(); }},
        {NodeType::Integer,    []() { return std::make_unique<IntegerNode>();    }},
        {NodeType::Float,      []() { return std::make_unique<FloatNode>();      }},
        {NodeType::Mesh,       []() { return std::make_unique<MeshNode>();       }},
        {NodeType::Shader,     []() { return std::make_unique<ShaderNode>();     }},
    };
    mNodeGroups = {
        {NodeGroup::Execution, {
            NodeType::Entry, NodeType::RenderPass,
        }},
        {NodeGroup::Numerics, {
            NodeType::Integer, NodeType::Float,
        }},
        {NodeGroup::Maths, {
            NodeType::Arithmetic,
        }},
        {NodeGroup::Graphics, {
            NodeType::Mesh, NodeType::Shader,
        }},
    };
}

void PipelineGraph::initializeDefault() {
    clearNodes();

    std::unique_ptr<EntryNode> defaultEntry = std::make_unique<EntryNode>(mPipelineHandler);
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

std::unique_ptr<Node> PipelineGraph::deserializeNodeType(unsigned int nodeType) {
    auto factory = mNodeFactories.find((NodeType)nodeType);
    return factory == mNodeFactories.end() ? nullptr : factory->second();
}

void PipelineGraph::drawNodeCreation() {
    static const std::unordered_map<NodeType, std::string> cNODE_NAMES = {
        {NodeType::Entry, "Entry"},
        {NodeType::Exit , "Exit" },
        {NodeType::RenderPass, "Render Pass"},

        {NodeType::Arithmetic, "Arithmetic"},

        {NodeType::Integer, "Integer"},
        {NodeType::Float  , "Float"  },

        {NodeType::Mesh  , "Mesh"  },
        {NodeType::Shader, "Shader"},
    };
    static const std::unordered_map<NodeGroup, std::string> cGROUP_NAMES {
        {NodeGroup::Execution, "Execution"},
        {NodeGroup::Maths    , "Maths"    },
        {NodeGroup::Numerics , "Numerics" },
        {NodeGroup::Graphics , "Graphics" },
    };

    for (const auto& group : mNodeGroups) {
        const std::string HEADER_LABEL = std::string(cGROUP_NAMES.find(group.first)->second).append("##GraphHeader");
        if (!ImGui::CollapsingHeader(HEADER_LABEL.c_str()))
            continue;

        for (const auto& nodeType : group.second) {
            const std::string NODE_LABEL = std::string(cNODE_NAMES.find(nodeType)->second).append("##GraphButton");
            if (ImGui::Selectable(NODE_LABEL.c_str()))
                addNode(mNodeFactories.find(nodeType)->second());
        }
    }
}
