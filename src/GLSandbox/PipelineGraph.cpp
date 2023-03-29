#include "PipelineGraph.h"

#include "../NodeEditor/Node.h"
#include "BasicNodes.h"
#include "MeshNode.h"
#include "ShaderNode.h"

#include <imgui.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>

PipelineGraph::PipelineGraph() {
    ShaderNode::findVertexFiles();
    ShaderNode::findFragmentFiles();
    ShaderNode::findTessContFiles();
    ShaderNode::findTessEvalFiles();
    ShaderNode::findGeometryFiles();
}

void PipelineGraph::drawNodeCreation() {
    struct NodeFactory {
        typedef std::function<std::unique_ptr<Node>()> factory_callback;

        NodeFactory(const std::string& label, factory_callback callback) : mCallback(std::move(callback)) {
            static int cLabelIDCounter = 0;
            mLabel = std::string(label).append("##NodeCreator_").append(std::to_string(cLabelIDCounter++));
        }

        std::string mLabel;
        factory_callback mCallback;
    };

    if (ImGui::CollapsingHeader("Maths##MeshHeader")) {
        static const NodeFactory cMATH_FACTORIES[] = {
            NodeFactory("Integer", []() { return std::make_unique<IntegerNode>(); }),
            NodeFactory("Float"  , []() { return std::make_unique<FloatNode>();   }),
        };
        for (const auto& nodeFactory : cMATH_FACTORIES) {
            if (ImGui::Selectable(nodeFactory.mLabel.c_str())) {
                addNode(nodeFactory.mCallback());
            }
        }
    }
    if (ImGui::CollapsingHeader("Graphics##MeshHeader")) {
        static const NodeFactory cGRAPHICS_FACTORIES[] = {
            NodeFactory("Mesh"  , []() { return std::make_unique<MeshNode>();  }),
            NodeFactory("Shader", []() { return std::make_unique<ShaderNode>();}),
        };
        for (const auto& nodeFactory : cGRAPHICS_FACTORIES) {
            if (ImGui::Selectable(nodeFactory.mLabel.c_str())) {
                addNode(nodeFactory.mCallback());
            }
        }
    }
}
