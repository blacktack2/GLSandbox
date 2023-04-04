#include "Node.h"

#include <imgui.h>
#include <imnodes.h>

#include <utility>

static int gNodeIDCounter = 1;

Node::Node(std::string title) : mTitle(std::move(title)), mID{gNodeIDCounter++} {
    ImNodes::SetNodeScreenSpacePos(getID(), ImVec2(0.0f, 0.0f));
}

void Node::serialize(std::ofstream& streamOut) const {
    streamOut << getID() << "\n";

    glm::vec2 position = getAbsolutePositionC();
    streamOut << position.x << "\n";
    streamOut << position.y << "\n";

    for (InPort& port : mInPorts) {
        streamOut << port.getID() << "\n";
        streamOut << port.getLinkedPortID() << "\n";
    }

    for (OutPort& port : mOutPorts) {
        streamOut << port.getID() << "\n";
    }

    serializeContents(streamOut);
}

void Node::deserialize(std::ifstream& streamIn, std::unordered_map<int, std::reference_wrapper<OutPort>>& outPorts,
                       std::vector<std::pair<std::reference_wrapper<InPort>, int>>& links) {
    int id;
    streamIn >> id;
    if (id != -1)
        mID = id;

    float posX, posY;
    streamIn >> posX;
    streamIn >> posY;
    setAbsolutePosition({posX, posY});

    for (InPort& port : mInPorts) {
        int portID, linkID;
        streamIn >> portID;
        streamIn >> linkID;
        if (linkID != -1)
            links.emplace_back(port, linkID);
    }

    for (OutPort& port : mOutPorts) {
        int portID;
        streamIn >> portID;
        outPorts.emplace(portID, port);
    }

    deserializeContents(streamIn);
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

void Node::addPort(IPort& port) {
    mPorts.emplace_back(port);
    if (InPort* inPort = dynamic_cast<InPort*>(&port))
        mInPorts.emplace_back(*inPort);
    else if (OutPort* outPort = dynamic_cast<OutPort*>(&port))
        mOutPorts.emplace_back(*outPort);
    else
        assert(false); // Port must be an input or an output
}

void Node::removePort(IPort& port) {
    mPorts.erase(std::remove_if(mPorts.begin(), mPorts.end(),
                               [&port](const auto& other) { return port.getID() == other.get().getID(); }),
                 mPorts.end());
    mInPorts.erase(std::remove_if(mInPorts.begin(), mInPorts.end(),
                               [&port](const auto& other) { return port.getID() == other.get().getID(); }),
                   mInPorts.end());
    mOutPorts.erase(std::remove_if(mOutPorts.begin(), mOutPorts.end(),
                               [&port](const auto& other) { return port.getID() == other.get().getID(); }),
                    mOutPorts.end());
}
