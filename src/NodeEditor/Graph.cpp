#include "Graph.h"

#include <imnodes.h>

Graph::Graph() {
    mContext = ImNodes::CreateContext();
}

Graph::~Graph() {
    ImNodes::DestroyContext(mContext);
}

void Graph::draw() {
    drawEditor();

    drawConfig();
}

void Graph::drawEditor() {
    ImGui::Begin("Editor", nullptr);
    ImNodes::BeginNodeEditor();

    for (auto& node : mNodes)
        node->draw();

    ImNodes::EndNodeEditor();
    ImGui::End();
}

void Graph::drawConfig() {
    ImGui::Begin("Config", &mConfigPanelOpen);

    ImGui::Text("Nodes");
    drawNodeCreation();

    ImGui::End();
}
