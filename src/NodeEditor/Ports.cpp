#include "Ports.h"

#include "Node.h"

#include <imgui.h>
#include <imnodes.h>

static int gPortIDCounter = 1;

Port::Port(const Node& parent, std::string displayName, int id) :
mParentNode(parent), mDisplayName(std::move(displayName)), mID{id == 0 ? gPortIDCounter : id} {
    gPortIDCounter = std::max(gPortIDCounter, mID + 1);
}

OutPort::OutPort(const Node& parent, const std::string& displayName, const std::any& value, int id) :
Port(parent, displayName, id), mValue(value) {
}

void OutPort::draw() const {
    ImGui::Dummy(ImVec2(0, 0));
    std::string text = getDisplayName() + " ->";
//    ImGui::SameLine(
//        ImGui::GetCursorPosX() + ax::NodeEditor::GetNodeSize(getParentNode().getID()).x -
//        ax::NodeEditor::GetNodePosition(getParentNode().getID()).x -
//        ImGui::CalcTextSize(text.c_str()).x - ImGui::GetStyle().ItemSpacing.x
//    );
    ImNodes::BeginOutputAttribute(getID());

    ImGui::Text("%s", text.c_str());

    ImNodes::EndOutputAttribute();
}

InPort::InPort(const Node& parent, const std::string& displayName, int id) :
Port(parent, displayName, id) {
}

void InPort::draw() const {
    ImNodes::BeginInputAttribute(getID());

    ImGui::Text("<- %s", getDisplayName().c_str());

    ImNodes::EndInputAttribute();

    if (mLink.has_value())
        ImNodes::Link(mLinkID, mLink.value().get().getID(), getID());
}
