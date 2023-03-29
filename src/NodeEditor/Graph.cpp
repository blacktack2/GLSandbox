#include "Graph.h"

#include <imgui_node_editor.h>

Graph::Graph() {
    mContext = ax::NodeEditor::CreateEditor();
}

void Graph::draw() {
    drawEditor();

    drawConfig();
}

void Graph::drawEditor() {
    ImGui::Begin("Editor", nullptr);

    ax::NodeEditor::SetCurrentEditor(mContext);
    ax::NodeEditor::Begin("Graph");

    for (auto& node : mNodes)
        node->draw();

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);

    ImGui::End();
}

void Graph::drawConfig() {
    ImGui::Begin("Config", &mConfigPanelOpen);

    ImGui::Text("Nodes");
    drawNodeCreation();

    ImGui::End();
}
