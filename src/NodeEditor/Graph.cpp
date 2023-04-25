#include "Graph.h"

#include "Ports.h"
#include "../Utils/SerializationUtils.h"

#include <imgui_node_editor.h>

static constexpr char gSERIAL_MARK_NODE[] = "Node";

static constexpr char gSERIAL_DATA_PREFIX = '^';
static constexpr char gSERIAL_DATA_NODE_TYPE[] = "NodeType";

void deserializeLinks(Node::port_data_t& portData, Node::link_data_t& linkData) {
    for (auto& dynamicLinkPair : linkData) {
        Node* nodeFrom = dynamicLinkPair.first;
        const std::vector<std::pair<std::string, int>>& nodeLinks = dynamicLinkPair.second;
        for (const auto& link : nodeLinks) {
            const auto match = portData.find(link.second);
            if (match == portData.end())
                continue;
            IPort* portA = nodeFrom->getPortByName(link.first);
            IPort* portB = match->second.first->getPortByName(match->second.second);
            if (portA && portB)
                portA->link(*portB);
        }
    }
}

Graph::Graph() {
    mContext = ed::CreateEditor();
}

Graph::~Graph() {
    ed::DestroyEditor(mContext);
}

void Graph::serialize(std::ofstream& streamOut) const {
    for (const auto& node : mNodes) {
        SerializationUtils::writeBeginMark(streamOut, gSERIAL_MARK_NODE);
        SerializationUtils::writeDataPoint(streamOut, gSERIAL_DATA_PREFIX, gSERIAL_DATA_NODE_TYPE,
                                           getNodeSerialName(*node));
        node->serialize(streamOut);
        SerializationUtils::writeEndMark(streamOut, gSERIAL_MARK_NODE);
    }
}

void Graph::deserialize(std::ifstream& streamIn) {
    mNodes.clear();

    Node::port_data_t staticPortData, dynamicPortData;
    Node::link_data_t staticLinkData, dynamicLinkData;

    std::streampos end = SerializationUtils::findEnd(streamIn);

    std::streampos markBegin, markEnd;
    while (SerializationUtils::findNextMarkPair(streamIn, end, gSERIAL_MARK_NODE, markBegin, markEnd)) {
        streamIn.seekg(markBegin);

        std::string nodeType;

        std::string dataID;
        while (SerializationUtils::seekNextDataPoint(streamIn, markEnd, gSERIAL_DATA_PREFIX, dataID)) {
            if (dataID == gSERIAL_DATA_NODE_TYPE) {
                streamIn >> nodeType;
                continue;
            }
            SerializationUtils::skipToNextLine(streamIn);
        }

        std::unique_ptr<Node> node = deserializeNodeType(nodeType);

        streamIn.seekg(markBegin);
        if (node) {
            node->deserialize(streamIn, markEnd, staticPortData, staticLinkData, dynamicPortData, dynamicLinkData);
            addNode(std::move(node));
        }
        streamIn.seekg(markEnd);
    }

    deserializeLinks(staticPortData, staticLinkData);
    deserializeLinks(staticPortData, dynamicLinkData);
    deserializeLinks(dynamicPortData, staticLinkData);
    deserializeLinks(dynamicPortData, dynamicLinkData);
}

void Graph::preDraw() {

}

void Graph::draw() {
    drawEditor();

    drawConfig();
}

void Graph::postDraw() {

}

void Graph::markDirty() {
    mIsDirty = true;
    for (const auto& event : mOnDirtyEvents)
        event();
}

void Graph::markClean() {
    mIsDirty = false;
    for (const auto& event : mOnCleanEvents)
        event();
    for (const auto& node : mNodes)
        node->markClean();
}

void Graph::drawEditor() {
    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoCollapse);
    ed::SetCurrentEditor(mContext);
    ed::Begin("Graph");

    for (auto& node : mNodes)
        node->draw();
    for (auto& node : mNodes)
        node->drawLinks();

    if (ed::BeginCreate()) {
        checkLinkCreated();
    }
    ed::EndCreate();
    if (ed::BeginDelete()) {
        checkLinksDeleted();
        checkNodesDeleted();
    }
    ed::EndDelete();

    ed::End();
    ed::SetCurrentEditor(nullptr);
    ImGui::End();
}

void Graph::drawConfig() {
    ImGui::Begin("Config", &mConfigPanelOpen);

    ImGui::Text("Nodes");
    drawNodeCreation();

    ImGui::End();
}

void Graph::checkLinkCreated() {
    ed::PinId portAID, portBID;
    if (!ed::QueryNewLink(&portAID, &portBID) || !ed::AcceptNewItem())
        return;

    IPort* portA = nullptr;
    IPort* portB  = nullptr;
    for (auto& node : mNodes) {
        IPort* in = node->getPort(portAID.Get());
        IPort* out = node->getPort(portBID.Get());
        if (!portA && out)
            portA = out;
        if (!portB && in)
            portB = in;
    }

    if (portA && portB && !portA->getParent().isLocked() & !portB->getParent().isLocked()) {
        portB->link(*portA);
        markDirty();
    }
}

void Graph::checkLinksDeleted() {
    ed::LinkId linkID;
    if (!ed::QueryDeletedLink(&linkID) || !ed::AcceptDeletedItem())
        return;

    for (auto& node : mNodes) {
        for (size_t i = 0; i < node->numPorts(); i++) {
            IPort& port = node->getPortByIndex(i);
            if (port.isLinked() && port.getLinkID() == linkID.Get() && !port.getParent().isLocked() && !port.getLinkedParent().isLocked()) {
                port.unlink();
                markDirty();
                return;
            }
        }
    }
}

void Graph::checkNodesDeleted() {
    ed::NodeId nodeId;
    if (!ed::QueryDeletedNode(&nodeId))
        return;

    size_t initial = mNodes.size();
    mNodes.erase(
        std::remove_if(
            mNodes.begin(), mNodes.end(),
            [nodeId](const auto& node) {
                return !node->isLocked() && node->getID() == nodeId.Get();
            }
        ),
        mNodes.end()
    );
    if (mNodes.size() != initial)
        markDirty();
}
