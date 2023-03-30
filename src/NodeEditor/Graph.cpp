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

    if (ax::NodeEditor::BeginCreate()) {
        ax::NodeEditor::PinId inputPinID, outputPinID;
        if (ax::NodeEditor::QueryNewLink(&inputPinID, &outputPinID) && inputPinID && outputPinID && ax::NodeEditor::AcceptNewItem()) {
            InPort* input   = nullptr;
            OutPort* output = nullptr;
            for (auto& node : mNodes) {
                for (size_t i = 0; i < node->numPorts(); i++) {
                    IPort& port = node->getPort(i);
                    if (!input)
                        input = dynamic_cast<InPort*>(&port);
                    if (!output)
                        output = dynamic_cast<OutPort*>(&port);
                }
            }
            if (input && output)
                input->link(*output);
        }
    }
    ax::NodeEditor::EndCreate();

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
