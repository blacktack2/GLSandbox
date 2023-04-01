#include "Graph.h"

#include <imnodes.h>

Graph::Graph() {
    mContext = ImNodes::CreateContext();
    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
}

Graph::~Graph() {
    ImNodes::DestroyContext(mContext);
}

void Graph::preDraw() {

}

void Graph::draw() {
    drawEditor();

    drawConfig();
}

void Graph::postDraw() {
    checkLinkCreated();
    checkLinkDestroyed();
}

void Graph::drawEditor() {
    ImGui::Begin("Editor", nullptr);
    ImNodes::BeginNodeEditor();

    for (auto& node : mNodes)
        node->draw();
    for (auto& node : mNodes)
        node->drawLinks();

    ImNodes::MiniMap();
    ImNodes::EndNodeEditor();
    ImGui::End();
}

void Graph::drawConfig() {
    ImGui::Begin("Config", &mConfigPanelOpen);

    ImGui::Text("Nodes");
    drawNodeCreation();

    ImGui::End();
}

void Graph::checkLinkCreated() {
    int outputID, inputID;
    if (!ImNodes::IsLinkCreated(&outputID, &inputID))
        return;

    OutPort* output = nullptr;
    InPort*  input  = nullptr;
    for (auto& node : mNodes) {
        output = output ? output : dynamic_cast<OutPort*>(node->getPort(outputID));
        input  = input  ? input  : dynamic_cast<InPort* >(node->getPort(inputID ));
    }

    if (output && input)
        input->link(*output);
}

void Graph::checkLinkDestroyed() {
    int linkID;
    if (!ImNodes::IsLinkDestroyed(&linkID))
        return;

    for (auto& node : mNodes) {
        for (size_t i = 0; i < node->numPorts(); i++) {
            IPort& port = node->getPortByIndex(i);
            if (port.getLinkID() == linkID) {
                port.unlink();
                return;
            }
        }
    }
}
