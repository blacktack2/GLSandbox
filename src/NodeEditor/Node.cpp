#include "Node.h"

#include <imgui/imgui.h>
#include <imgui/imnodes.h>

#include <utility>

static int idCounter = 1;

Connection::Connection(Node& from, Node& to) : mFromNode(from), mToNode(to) {

}

Port::Port() {

}

void Port::draw() {

}

InPort::InPort() {

}

OutPort::OutPort() {

}

Node::Node(std::string title, int id) : mTitle(std::move(title)), mID{id == 0 ? idCounter : id} {
    idCounter = std::max(idCounter, mID + 1);
}

void Node::draw() {
    ImNodes::BeginNode(mID);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", mTitle.c_str());
    ImNodes::EndNodeTitleBar();
    drawContents();
    ImNodes::EndNode();
}

void Node::run() {
    runContents();
}
