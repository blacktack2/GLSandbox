#include "Graph.h"

#include "Ports.h"
#include "../Utils/SerializationUtils.h"

#include <imgui_node_editor.h>

static constexpr char gSERIAL_MARK_NODE[] = "Node";

static constexpr char gSERIAL_DATA_PREFIX = '^';
static constexpr char gSERIAL_DATA_NODE_TYPE[] = "NodeType";

bool deserializeLinks(const Node::port_data_t& portData, const Node::link_data_t& linkData) {
    bool changed = false;
    for (const auto& linkPair : linkData) {
        Node* nodeFrom = linkPair.first;
        const std::vector<std::pair<std::string, int>>& nodeLinks = linkPair.second;

        for (const std::pair<std::string, int>& link : nodeLinks) {
            std::string portNameFrom = link.first;
            int portID = link.second;
            const auto match = portData.find(portID);
            if (match == portData.end())
                continue;

            Node* nodeTo = match->second.first;
            std::string portNameTo = match->second.second;

            IPort* portFrom = nodeFrom->getPortByName(portNameFrom);
            IPort* portTo = nodeTo->getPortByName(portNameTo);

            if (portFrom && portTo && !(portFrom->isLinkedWith(*portTo) || portTo->isLinkedWith(*portFrom)))
                changed |= portFrom->link(*portTo);
        }
    }
    return changed;
}

Graph::Graph() {
    mContext = ed::CreateEditor();
}

Graph::~Graph() {
    ed::DestroyEditor(mContext);
}

void Graph::safeDestroy() {
    for (const auto& node : mNodes)
        node->safeDestroy();
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
    onClear();
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
        if (dataID.empty()) {
            streamIn.seekg(markEnd);
            continue;
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
    bool dynamicUpdated;
    do {
        dynamicUpdated = false;
        dynamicUpdated |= deserializeLinks(staticPortData, dynamicLinkData);
        dynamicUpdated |= deserializeLinks(dynamicPortData, staticLinkData);
        dynamicUpdated |= deserializeLinks(dynamicPortData, dynamicLinkData);
    } while (dynamicUpdated);
}

void Graph::preDraw() {

}

void Graph::draw() {
    drawEditor();

    drawConfig();

    drawInputPanel();
    drawOutputPanel();
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

void Graph::addNode(std::unique_ptr<Node> node) {
    if (node) {
        node->setParent(this);
        onNodeAdd(*node);
        mNodes.push_back(std::move(node));
        markDirty();
    }
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

void Graph::drawInputPanel() {
    ImGui::Begin("Inputs");

    drawInputPanelContents();

    ImGui::End();
}

void Graph::drawOutputPanel() {
    ImGui::Begin("Outputs");

    drawOutputPanelContents();

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
            for (size_t j = 0; j < port.getNumLinks(); j++) {
                if (port.getLinkID(j) != linkID.Get() || port.getParent().isLocked() || port.getLinkedParent(j).isLocked())
                    continue;
                port.unlink(linkID.Get());
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
            [this, nodeId](const auto& node) {
                if (!node->isLocked() && node->getID() == nodeId.Get()) {
                    onNodeDelete(*node);
                    node->safeDestroy();
                    return true;
                }
                return false;
            }
        ),
        mNodes.end()
    );
    if (mNodes.size() != initial)
        markDirty();
}
