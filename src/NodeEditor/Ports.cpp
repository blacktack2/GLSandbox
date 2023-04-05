#include "Ports.h"

#include "Node.h"

#include <imgui.h>
#include <imnodes.h>

#include <utility>

static int gPortIDCounter = 1;

Port::Port() : mParentNode(nullptr), mUniqueName("INVALID"), mDisplayName("INVALID"), mID(-1) {

}

Port::Port(const Node& parent, const std::string& uniqueName, std::string displayName) :
mParentNode(&parent), mUniqueName(uniqueName), mDisplayName(std::move(displayName)), mID{gPortIDCounter++} {
}

OutPort::OutPort() : Port(), mGetValue(nullptr) {

}

OutPort::OutPort(const Node& parent, const std::string& uniqueName, const std::string& displayName, get_node_value_callback getValue) :
Port(parent, uniqueName, displayName), mGetValue(std::move(getValue)) {
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

std::any OutPort::getLinkValue() const {
    return mGetValue();
}

bool OutPort::isLinked() const {
    return mLink != nullptr;
}

int OutPort::getLinkID() const {
    return mLink ? mLink->getLinkID() : -1;
}

void OutPort::unlink() {
    if (!mLink)
        return;
    mLink->unlinkSoft();
    mLink = nullptr;

    onUnlink();
}

void OutPort::link(InPort& port) {
    if (!port.isConnectionValid(*this))
        return;
    if (mLink) {
        unlink();
    }
    linkSoft(port);
    mLink->linkSoft(*this);

    onLink();
}

void OutPort::unlinkSoft() {
    mLink = nullptr;

    onUnlink();
}

void OutPort::linkSoft(InPort& port) {
    static int cLinkIDCounter = 1;
    mLink = &port;

    onLink();
}

const Node& OutPort::getLinkParent() const {
    return mLink->getParent();
}

Port* OutPort::getLinkedPort() {
    return mLink;
}

InPort::InPort() : Port(), mValidConnections() {

}

InPort::InPort(const Node& parent, const std::string& uniqueName, const std::string& displayName,
               std::vector<std::type_index> validConnections) :
Port(parent, uniqueName, displayName), mValidConnections(std::move(validConnections)) {
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

std::any InPort::getLinkValue() const {
    return mLink->getLinkValue();
}

bool InPort::isLinked() const {
    return mLink != nullptr;
}

int InPort::getLinkID() const {
    return mLink ? mLinkID : -1;
}

int InPort::getLinkedPortID() const {
    return mLink ? mLink->getID() : -1;
}

void InPort::unlink() {
    if (!mLink)
        return;

    mLink->unlinkSoft();
    mLink = nullptr;

    onUnlink();
}

void InPort::link(OutPort& port) {
    if (!isConnectionValid(port))
        return;
    if (mLink) {
        unlink();
    }
    linkSoft(port);
    mLink->linkSoft(*this);

    onLink();
}

void InPort::unlinkSoft() {
    mLink = nullptr;

    onUnlink();
}

void InPort::linkSoft(OutPort& port) {
    static int cLinkIDCounter = 1;
    mLink = &port;
    mLinkID = cLinkIDCounter++;

    onLink();
}

bool InPort::isConnectionValid(const IPort& port) {
    if (mValidConnections.empty())
        return true;
    const std::type_index type = port.getLinkValue().type();
    return std::any_of(mValidConnections.begin(), mValidConnections.end(),
                       [&type](const std::type_index other) { return other == type; });
}

Port* InPort::getLinkedPort() {
    return mLink;
}
