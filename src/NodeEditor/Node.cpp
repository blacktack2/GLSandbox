#include "Node.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <utility>

static int gNodeIDCounter = 1;

Node::Node(std::string title, int id) : mTitle(std::move(title)), mID{id == 0 ? gNodeIDCounter : id} {
    gNodeIDCounter = std::max(gNodeIDCounter, mID + 1);
}

void Node::draw() {
    ax::NodeEditor::BeginNode(mID);

    ImGui::Text("%s", mTitle.c_str());

    for (auto port : mPorts)
        port.get().draw();

    drawContents();

    ax::NodeEditor::EndNode();
}
