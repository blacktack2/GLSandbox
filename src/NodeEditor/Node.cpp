#include "Node.h"

#include <imgui/imgui.h>
#include <imgui/imnodes.h>

#include <utility>

static int gNodeIDCounter = 1;

Node::Node(std::string title, int id) : mTitle(std::move(title)), mID{id == 0 ? gNodeIDCounter : id} {
    gNodeIDCounter = std::max(gNodeIDCounter, mID + 1);
}

void Node::draw() {
    ImNodes::BeginNode(mID);

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", mTitle.c_str());
    ImNodes::EndNodeTitleBar();

    for (auto port : mPorts)
        port.get().draw();

    drawContents();

    ImNodes::EndNode();
}
