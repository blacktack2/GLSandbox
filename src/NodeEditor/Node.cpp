#include "Node.h"

#include "Graph.h"
#include "Ports.h"
#include "../Utils/SerializationUtils.h"

#include <imgui.h>
#include <imgui_node_editor.h>
#include <imgui_stdlib.h>

#include <utility>

namespace ed = ax::NodeEditor;

int gGraphIDCounter = 1;

static constexpr char gSERIAL_MARK_PREFIX = '%';
static constexpr char gSERIAL_MARK_DATA[] = "Data";

static constexpr char gSERIAL_DATA_PREFIX = '-';
static constexpr char gSERIAL_DATA_ID[] = "NodeID";
static constexpr char gSERIAL_DATA_NAME[] = "NodeName";
static constexpr char gSERIAL_DATA_POSITION[] = "NodePosition";
static constexpr char gSERIAL_DATA_INPORT[] = "InPort";
static constexpr char gSERIAL_DATA_OUTPORT[] = "OutPort";
static constexpr char gSERIAL_DATA_DYNAMIC_INPORT[] = "Dynamic-InPort";
static constexpr char gSERIAL_DATA_DYNAMIC_OUTPORT[] = "Dynamic-OutPort";

static constexpr char gSERIAL_SUBDATA_PREFIX = '-';

void writeDataPoints(std::ofstream& streamOut, char prefix,
                     const std::vector<std::pair<std::string, std::string>>& dataPoints) {
    for (const auto& dataPair : dataPoints)
        SerializationUtils::writeDataPoint(streamOut, prefix, dataPair.first, dataPair.second);
}

Node::Node(std::string title) : mName(std::move(title)), mID{gGraphIDCounter++} {
    setRelativePosition(ImVec2(50.0f, 50.0f));
}

Node::~Node() {
    ImUtils::softUnsetDataPanel(*this);
}

void Node::setParent(Graph* parent) {
    mParent = parent;
}

void Node::serialize(std::ofstream& streamOut) const {
    std::string serializedID = std::to_string(getID());
    ImVec2 position = getAbsolutePosition();
    std::string serializedPosition = std::to_string(position.x).append(" ").append(std::to_string(position.y));

    std::vector<std::pair<std::string, std::string>> data{};

    data.emplace_back(gSERIAL_DATA_ID, serializedID);
    data.emplace_back(gSERIAL_DATA_NAME, mName);
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
        if (dataID == gSERIAL_DATA_NAME) {
            streamIn >> mName;
        } else if (dataID == gSERIAL_DATA_POSITION) {
            ImVec2 position;
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
    ed::BeginNode(mID);

    ImUtils::nodeDummy();

    if (mIsPositionDirty) {
        ed::SetNodePosition(getID(), mIsPositionRelative ? ed::ScreenToCanvas(mPosition) : mPosition);
        mIsPositionDirty = false;
    }
    mPosition = ed::GetNodePosition(getID());

    ImGui::BeginDisabled(mIsLocked);

    ImUtils::inputNodeName(mName, generateNodeLabelID("NodeName"), mIsDirty);

    ImGui::EndDisabled();

    for (size_t i = 0; i < std::max(mInPorts.size(), mOutPorts.size()); i++) {
        bool drawIn = mInPorts.size() > i;
        bool drawOut = mOutPorts.size() > i;
        if (drawIn)
            mInPorts[i].get().drawPort();
        else
            ImGui::Dummy(ImVec2(0.0f, 0.0f));

        if (drawOut)
            mOutPorts[i].get().drawPort();
        else
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
    }

    drawContents();

    ed::EndNode();
}

void Node::drawLinks() {
    for (auto port : mPorts)
        port.get().drawLink();
}

ImVec2 Node::getAbsolutePosition() const {
    return mPosition;
}

void Node::setAbsolutePosition(const ImVec2& pos) {
    mPosition = pos;
    mIsPositionDirty = true;
    mIsPositionRelative = false;
}

ImVec2 Node::getRelativePosition() const {
    return mPosition;
}

void Node::setRelativePosition(const ImVec2& pos) {
    mPosition = pos;
    mIsPositionDirty = true;
    mIsPositionRelative = true;
}

ImVec2 Node::getSize() const {
    return ed::GetNodeSize(getID());
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

void Node::markDirty() {
    mIsDirty = true;
    mParent->markDirty();
}

void Node::markClean() {
    mIsDirty = false;
}

void Node::lock() {
    if (mIsLocked)
        return;
    mIsLocked = true;
    for (auto port : mPorts)
        if (port.get().isLinked())
            port.get().getLinkedParent().lock();
    onLock();
}

void Node::unlock() {
    if (!mIsLocked)
        return;
    mIsLocked = false;
    for (auto port : mPorts)
        if (port.get().isLinked())
            port.get().getLinkedParent().unlock();
    onUnlock();
}
