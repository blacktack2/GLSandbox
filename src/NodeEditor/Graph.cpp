#include "Graph.h"

#include <imgui_node_editor.h>

Graph::Graph() {
    mContext = ax::NodeEditor::CreateEditor();
}

void Graph::draw() {
    ax::NodeEditor::SetCurrentEditor(mContext);
    ax::NodeEditor::Begin("Graph");

    for (auto& node : mNodes)
        node->draw();

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}
