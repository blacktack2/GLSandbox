#include "FixedNodes.h"

EntryNode::EntryNode() : Node("Entry") {
    addPort(mExecutionOutPort);
}

void EntryNode::drawContents() {

}

ExitNode::ExitNode() : Node("Exit") {
    addPort(mExecutionInPort);
}

void ExitNode::drawContents() {

}
