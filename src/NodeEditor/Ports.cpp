#include "Ports.h"

#include <imgui.h>
#include <imgui_node_editor.h>

static int gPortIDCounter = 1;

Port::Port(std::string displayName, int id) :
        mDisplayName(std::move(displayName)), mID{id == 0 ? gPortIDCounter : id} {
    gPortIDCounter = std::max(gPortIDCounter, mID + 1);
}

OutPort::OutPort(const std::string& displayName, const std::any& value, int id) : Port(displayName, id), mValue(value) {
}

void OutPort::draw() const {
    ax::NodeEditor::BeginPin(getID(), ax::NodeEditor::PinKind::Output);

    ImGui::Text("%s", getDisplayName().c_str());

    ax::NodeEditor::EndPin();
}

InPort::InPort(const std::string& displayName, int id) : Port(displayName, id) {
}

void InPort::draw() const {
    ax::NodeEditor::BeginPin(getID(), ax::NodeEditor::PinKind::Input);

    ImGui::Text("%s", getDisplayName().c_str());

    ax::NodeEditor::EndPin();

    if (mLink.has_value())
        ax::NodeEditor::Link(mLinkID, mLink.value().get().getID(), getID());
}
