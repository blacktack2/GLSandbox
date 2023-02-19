#include "PipelineGraph.h"

#include "../NodeEditor/Node.h"
#include "ShaderNode.h"

PipelineGraph::PipelineGraph() {
    ShaderNode::findVertexFiles();
    ShaderNode::findFragmentFiles();
    ShaderNode::findTessContFiles();
    ShaderNode::findTessEvalFiles();
    ShaderNode::findGeometryFiles();

    mNodes.push_back(std::make_unique<ShaderNode>());
}
