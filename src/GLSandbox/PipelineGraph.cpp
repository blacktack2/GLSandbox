#include "PipelineGraph.h"

#include "../NodeEditor/Node.h"
#include "BasicNodes.h"
#include "MeshNode.h"
#include "ShaderNode.h"

PipelineGraph::PipelineGraph() {
    ShaderNode::findVertexFiles();
    ShaderNode::findFragmentFiles();
    ShaderNode::findTessContFiles();
    ShaderNode::findTessEvalFiles();
    ShaderNode::findGeometryFiles();

//    addNode(std::move(std::make_unique<IntegerNode>()));
//    addNode(std::move(std::make_unique<FloatNode>()));
    addNode(std::move(std::make_unique<MeshNode>()));
//    addNode(std::move(std::make_unique<ShaderNode>()));
}
