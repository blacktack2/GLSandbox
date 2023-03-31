#include "ExecutableNodes.h"

EntryNode::EntryNode() : Node("Entry") {
    addPort(mExecutionOutPort);
}

void EntryNode::drawContents() {

}

RenderPassNode::RenderPassNode() : Node("Render Pass") {
    addPort(mExecutionInPort);
    addPort(mExecutionOutPort);
}

void RenderPassNode::drawContents() {

}

ExitNode::ExitNode() : Node("Exit") {
    addPort(mExecutionInPort);
}

void ExitNode::drawContents() {

}
