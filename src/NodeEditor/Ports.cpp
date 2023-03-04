#include "Ports.h"

#include <imgui/imgui.h>
#include <imgui/imnodes.h>

static int gPortIDCounter = 1;

Port::Port(std::string displayName, int id) :
        mDisplayName(std::move(displayName)), mID{id == 0 ? gPortIDCounter : id} {
    gPortIDCounter = std::max(gPortIDCounter, mID + 1);
}

OutPort::OutPort(const std::string& displayName, const std::any& value, int id) : Port(displayName, id), mValue(value) {
}

void OutPort::draw() const {
    ImNodes::BeginOutputAttribute(getID());

    ImGui::Text("%s", getDisplayName().c_str());

    ImNodes::EndOutputAttribute();
}

InPort::InPort(const std::string& displayName, int id) : Port(displayName, id) {
}

void InPort::draw() const {
    ImNodes::BeginInputAttribute(getID());

    ImGui::Text("%s", getDisplayName().c_str());

    ImNodes::EndInputAttribute();

    if (mLink.has_value())
        ImNodes::Link(mLinkID, mLink.value().get().getID(), getID());
}
