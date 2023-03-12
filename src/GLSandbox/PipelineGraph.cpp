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

//    mNodes.push_back(std::make_unique<IntegerNode>());
//    mNodes.push_back(std::make_unique<FloatNode>());
    mNodes.push_back(std::make_unique<MeshNode>());
//    mNodes.push_back(std::make_unique<ShaderNode>());
}
