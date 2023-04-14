#include "Node.h"

#include "Ports.h"
#include "../Utils/SerializationUtils.h"

#include <imgui.h>
#include <imnodes.h>

#include <utility>

int gNodeIDCounter = 1;

static constexpr char gSERIAL_MARK_PREFIX = '%';
static constexpr char gSERIAL_MARK_DATA[] = "Data";

static constexpr char gSERIAL_DATA_PREFIX = '-';
static constexpr char gSERIAL_DATA_ID[] = "NodeID";
static constexpr char gSERIAL_DATA_POSITION[] = "NodePosition";
static constexpr char gSERIAL_DATA_INPORT[] = "InPort";
static constexpr char gSERIAL_DATA_OUTPORT[] = "OutPort";
static constexpr char gSERIAL_DATA_DYNAMIC_INPORT[] = "Dynamic-InPort";
static constexpr char gSERIAL_DATA_DYNAMIC_OUTPORT[] = "Dynamic-OutPort";

static constexpr char gSERIAL_SUBDATA_PREFIX = '-';

Node::Node(std::string title) : mTitle(std::move(title)), mID{gNodeIDCounter++} {
    ImNodes::SetNodeScreenSpacePos(getID(), ImVec2(0.0f, 0.0f));
}

void Node::serialize(std::ofstream& streamOut) const {
    std::string serializedID = std::to_string(getID());
    glm::vec2 position = getAbsolutePositionC();
    std::string serializedPosition = std::to_string(position.x).append(" ").append(std::to_string(position.y));

    std::vector<std::pair<std::string, std::string>> data{};

    data.emplace_back(gSERIAL_DATA_ID, serializedID);
    data.emplace_back(gSERIAL_DATA_POSITION, serializedPosition);

    for (const IPort& port : mInPorts)
        data.emplace_back(port.isDynamic() ? gSERIAL_DATA_DYNAMIC_INPORT : gSERIAL_DATA_INPORT,
                          std::string(port.getUniqueName()).append(" ").append(std::to_string(port.getLinkedPortID())));
    for (const IPort& port : mOutPorts)
        data.emplace_back(port.isDynamic() ? gSERIAL_DATA_DYNAMIC_OUTPORT : gSERIAL_DATA_OUTPORT,
                          std::string(port.getUniqueName()).append(" ").append(std::to_string(port.getID())));

    writeDataPoints(streamOut, gSERIAL_DATA_PREFIX, data);

    SerializationUtils::writeBeginMark(streamOut, gSERIAL_MARK_DATA);
    writeDataPoints(streamOut, gSERIAL_SUBDATA_PREFIX, generateSerializedData());
    SerializationUtils::writeEndMark(streamOut, gSERIAL_MARK_DATA);
}

void Node::deserialize(std::ifstream& streamIn, std::streampos end,
                       port_data_t& staticPortData, link_data_t& staticLinkData,
                       port_data_t& dynamicPortData, link_data_t& dynamicLinkData) {
    std::streampos begin = streamIn.tellg();

    std::string dataID;
    while (SerializationUtils::seekNextDataPoint(streamIn, end, gSERIAL_DATA_PREFIX, dataID)) {
        if (dataID == gSERIAL_DATA_ID) {
            streamIn >> dataID;
        } else if (dataID == gSERIAL_DATA_POSITION) {
            glm::vec2 position;
            streamIn >> position.x;
            streamIn >> position.y;
            setAbsolutePosition(position);
        } else if (dataID == gSERIAL_DATA_INPORT) {
            std::string uniqueName;
            int linkID;
            streamIn >> uniqueName;
            streamIn >> linkID;
            staticLinkData[this].emplace_back(uniqueName, linkID);
        } else if (dataID == gSERIAL_DATA_OUTPORT) {
            std::string uniqueName;
            int id;
            streamIn >> uniqueName;
            streamIn >> id;
            staticPortData.emplace(id, std::make_pair(this, uniqueName));
        } else if (dataID == gSERIAL_DATA_DYNAMIC_INPORT) {
            std::string uniqueName;
            int linkID;
            streamIn >> uniqueName;
            streamIn >> linkID;
            dynamicLinkData[this].emplace_back(uniqueName, linkID);
        } else if (dataID == gSERIAL_DATA_DYNAMIC_OUTPORT) {
            std::string uniqueName;
            int id;
            streamIn >> uniqueName;
            streamIn >> id;
            dynamicPortData.emplace(id, std::make_pair(this, uniqueName));
        } else {
            SerializationUtils::skipToNextLine(streamIn);
        }
    }

    streamIn.seekg(begin);
    std::streampos markBegin, markEnd;
    while (SerializationUtils::findNextMarkPair(streamIn, end, gSERIAL_MARK_DATA, markBegin, markEnd)) {
        streamIn.seekg(markBegin);
        std::string subdataID;
        while (SerializationUtils::seekNextDataPoint(streamIn, markEnd, gSERIAL_SUBDATA_PREFIX, subdataID)) {
            std::streampos current = streamIn.tellg();
            deserializeData(subdataID, streamIn);
            streamIn.seekg(current);
            SerializationUtils::skipToNextLine(streamIn);
        }
        streamIn.seekg(markEnd);
    }
    streamIn.seekg(end);

    onDeserialize();
}

void Node::draw() {
    ImNodes::BeginNode(mID);

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s", mTitle.c_str());
    ImNodes::EndNodeTitleBar();

    for (auto& port : mPorts)
        port.get().drawPort();

    drawContents();

    ImNodes::EndNode();
}

void Node::drawLinks() {
    for (auto port : mPorts)
        port.get().drawLink();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-make-member-function-const"
void Node::setAbsolutePosition(const glm::vec2& pos) {
    ImNodes::SetNodeEditorSpacePos(getID(), ImVec2(pos.x, pos.y));
}

void Node::setRelativePosition(const glm::vec2& pos) {
    ImNodes::SetNodeScreenSpacePos(getID(), ImVec2(pos.x, pos.y));
}
#pragma clang diagnostic pop

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
    switch (port.getDirection()) {
        case IPort::Direction::In  : mInPorts.emplace_back(port) ; break;
        case IPort::Direction::Out : mOutPorts.emplace_back(port); break;
    }
}

void Node::removePort(const IPort& port) {
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

size_t Node::numPorts() const {
    return mPorts.size();
}

IPort* Node::getPortByName(const std::string& uniqueName) {
    for (auto& port : mPorts)
        if (port.get().getUniqueName() == uniqueName)
            return &port.get();
    return nullptr;
}

IPort& Node::getPortByIndex(size_t i) {
    return mPorts[i].get();
}

IPort* Node::getPort(int portID) {
    for (auto& port : mPorts)
        if (port.get().getID() == portID)
            return &port.get();
    return nullptr;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
void Node::writeDataPoints(std::ofstream& streamOut, char prefix,
                           const std::vector<std::pair<std::string, std::string>>& dataPoints) const {
    for (const auto& dataPair : dataPoints)
        SerializationUtils::writeDataPoint(streamOut, prefix, dataPair.first, dataPair.second);
}
#pragma clang diagnostic pop
