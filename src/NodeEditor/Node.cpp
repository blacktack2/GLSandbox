#include "Node.h"

#include <imgui.h>
#include <imnodes.h>

#include <utility>

static int gNodeIDCounter = 1;

Node::Node(std::string title, int id) : mTitle(std::move(title)), mID{id == 0 ? gNodeIDCounter : id} {
    gNodeIDCounter = std::max(gNodeIDCounter, mID + 1);
    ImNodes::SetNodeEditorSpacePos(getID(), ImVec2(0.0f, 0.0f));
}

void Node::draw() {
    ImNodes::BeginNode(mID);

    ImGui::Text("%s", mTitle.c_str());

    for (auto port : mPorts)
        port.get().draw();

    drawContents();

    ImNodes::EndNode();
}

void Node::drawLinks() {
    for (auto port : mPorts)
        port.get().drawLinks();
}

void Node::setAbsolutePosition(const glm::vec2& pos) {
    ImNodes::SetNodeEditorSpacePos(getID(), ImVec2(pos.x, pos.y));
}

void Node::setRelativePosition(const glm::vec2& pos) {
    ImNodes::SetNodeScreenSpacePos(getID(), ImVec2(pos.x, pos.y));
}

glm::vec2 Node::getAbsolutePositionC() const {
    ImVec2 absCenter = ImNodes::GetNodeEditorSpacePos(getID());
    return glm::vec2(absCenter.x, absCenter.y);
}

glm::vec2 Node::getAbsolutePositionTR() const {
    ImVec2 absCenter = ImNodes::GetNodeEditorSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(absCenter.x + size.x * 0.5f, absCenter.y + size.y * 0.5f);
}

glm::vec2 Node::getAbsolutePositionBR() const {
    ImVec2 absCenter = ImNodes::GetNodeEditorSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(absCenter.x + size.x * 0.5f, absCenter.y - size.y * 0.5f);
}

glm::vec2 Node::getAbsolutePositionBL() const {
    ImVec2 absCenter = ImNodes::GetNodeEditorSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(absCenter.x - size.x * 0.5f, absCenter.y - size.y * 0.5f);
}

glm::vec2 Node::getAbsolutePositionTL() const {
    ImVec2 absCenter = ImNodes::GetNodeEditorSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(absCenter.x - size.x * 0.5f, absCenter.y + size.y * 0.5f);
}

glm::vec2 Node::getRelativePositionC() const {
    ImVec2 relCenter = ImNodes::GetNodeScreenSpacePos(getID());
    return glm::vec2(relCenter.x, relCenter.y);
}

glm::vec2 Node::getRelativePositionTR() const {
    ImVec2 relCenter = ImNodes::GetNodeScreenSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(relCenter.x + size.x * 0.5f, relCenter.y + size.y * 0.5f);
}

glm::vec2 Node::getRelativePositionBR() const {
    ImVec2 relCenter = ImNodes::GetNodeScreenSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(relCenter.x + size.x * 0.5f, relCenter.y - size.y * 0.5f);
}

glm::vec2 Node::getRelativePositionBL() const {
    ImVec2 relCenter = ImNodes::GetNodeScreenSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(relCenter.x - size.x * 0.5f, relCenter.y - size.y * 0.5f);
}

glm::vec2 Node::getRelativePositionTL() const {
    ImVec2 relCenter = ImNodes::GetNodeScreenSpacePos(getID());
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(relCenter.x - size.x * 0.5f, relCenter.y + size.y * 0.5f);
}

glm::vec2 Node::getSize() const {
    ImVec2 size = ImNodes::GetNodeDimensions(getID());
    return glm::vec2(size.x, size.y);
}
