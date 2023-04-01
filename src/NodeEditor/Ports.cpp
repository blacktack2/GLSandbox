#include "Ports.h"

#include "Node.h"

#include <imgui.h>
#include <imnodes.h>

#include <utility>

static int gPortIDCounter = 1;

Port::Port(const Node& parent, std::string displayName, int id) :
mParentNode(parent), mDisplayName(std::move(displayName)), mID{id == 0 ? gPortIDCounter : id} {
    gPortIDCounter = std::max(gPortIDCounter, mID + 1);
}

OutPort::OutPort(const Node& parent, const std::string& displayName, int id) :
Port(parent, displayName, id) {
}

void OutPort::draw() const {
    ImGui::Dummy(ImVec2(0, 0));
    ImNodes::BeginOutputAttribute(getID());

    std::string text = getDisplayName() + " ->";
    ImGui::Text("%s", text.c_str());

    ImNodes::EndOutputAttribute();
}

void OutPort::drawLinks() const {
    // Link drawing handled by InPort
}

void OutPort::unlink() {
    if (mLink) {
        mLink->unlinkSoft();
        mLink = nullptr;
    }
}

void OutPort::link(InPort& port) {
    if (!port.isConnectionValid(*this))
        return;
    if (mLink) {
        unlink();
    }
    linkSoft(port);
    mLink->linkSoft(*this);
}

void OutPort::unlinkSoft() {
    mLink = nullptr;
}

void OutPort::linkSoft(InPort& port) {
    static int cLinkIDCounter = 1;
    mLink = &port;
}

InPort::InPort(const Node& parent, const std::string& displayName,
               std::vector<const std::type_info*> validConnections, int id) :
Port(parent, displayName, id), mValidConnections(std::move(validConnections)) {
}

void InPort::draw() const {
    ImNodes::BeginInputAttribute(getID());

    ImGui::Text("<- %s", getDisplayName().c_str());

    ImNodes::EndInputAttribute();
}

void InPort::drawLinks() const {
    if (mLink)
        ImNodes::Link(mLinkID, mLink->getID(), getID());
}

void InPort::unlink() {
    if (mLink) {
        mLink->unlinkSoft();
        mLink = nullptr;
    }
}

void InPort::link(OutPort& port) {
    if (!isConnectionValid(port))
        return;
    if (mLink) {
        unlink();
    }
    linkSoft(port);
    mLink->linkSoft(*this);
}

void InPort::unlinkSoft() {
    mLink = nullptr;
}

void InPort::linkSoft(OutPort& port) {
    static int cLinkIDCounter = 1;
    mLink = &port;
    mLinkID = cLinkIDCounter++;
}

bool InPort::isConnectionValid(const IPort& port) {
    if (mValidConnections.empty())
        return true;
    const Node& parent = port.getParent(); // Multiline to avoid typeid warning
    const std::type_info* check = &typeid(parent);
    return std::any_of(mValidConnections.begin(), mValidConnections.end(),
                       [check](const std::type_info* a) { return *a == *check; });
}

const Node* InPort::getInput() const {
    if (!mLink)
        return nullptr;
    return &mLink->getParent();
}
