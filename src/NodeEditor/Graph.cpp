#include "Graph.h"

#include "Ports.h"
#include "../Utils/SerializationUtils.h"

#include <imnodes.h>

static constexpr char gSERIAL_MARK_NODE[] = "Node";

static constexpr char gSERIAL_DATA_PREFIX = '^';
static constexpr char gSERIAL_DATA_NODE_TYPE[] = "Direction";

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
    mContext = ImNodes::CreateContext();
    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
}

Graph::~Graph() {
    ImNodes::DestroyContext(mContext);
}

void Graph::serialize(std::ofstream& streamOut) const {
    for (const auto& node : mNodes) {
        SerializationUtils::writeBeginMark(streamOut, gSERIAL_MARK_NODE);
        SerializationUtils::writeDataPoint(streamOut, gSERIAL_DATA_PREFIX, gSERIAL_DATA_NODE_TYPE,
                                           std::to_string(node->getTypeID()));
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

        unsigned int nodeType;

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
    checkPanning();
}

void Graph::draw() {
    drawEditor();

    drawConfig();
}

void Graph::postDraw() {
    checkLinkCreated();
    checkLinkDestroyed();
    checkLinksDeleted();
    checkNodesDeleted();
}

void Graph::checkPanning() {
    if (mIsPanning && (ImGui::GetIO().MouseReleased[1] || !ImGui::GetIO().MouseDown[1])) {
        mIsPanning = false;
        return;
    }

    if (mIsPanning) {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        ImNodes::EditorContextResetPanning(ImVec2(mPanOriginX + delta.x, mPanOriginY + delta.y));
    } else {
        if (!ImGui::GetIO().MouseClicked[1])
            return;
        mIsPanning = true;
        ImVec2 current = ImNodes::EditorContextGetPanning();
        mPanOriginX = current.x;
        mPanOriginY = current.y;
    }
}

void Graph::drawEditor() {
    ImGui::Begin("Editor", nullptr);
    ImNodes::BeginNodeEditor();

    for (auto& node : mNodes)
        node->draw();
    for (auto& node : mNodes)
        node->drawLinks();

    ImNodes::MiniMap();
    ImNodes::EndNodeEditor();
    ImGui::End();
}

void Graph::drawConfig() {
    ImGui::Begin("Config", &mConfigPanelOpen);

    ImGui::Text("Nodes");
    drawNodeCreation();

    ImGui::End();
}

void Graph::checkLinkCreated() {
    int outputID, inputID;
    if (!ImNodes::IsLinkCreated(&outputID, &inputID))
        return;

    IPort* output = nullptr;
    IPort*  input  = nullptr;
    for (auto& node : mNodes) {
        IPort* in = node->getPort(inputID);
        IPort* out = node->getPort(outputID);
        if (!output && out && out->getDirection() == IPort::Direction::Out)
            output = out;
        if (!input && in && in->getDirection() == IPort::Direction::In)
            input = in;
    }

    if (output && input)
        input->link(*output);
}

void Graph::checkLinkDestroyed() {
    int linkID;
    if (!ImNodes::IsLinkDestroyed(&linkID))
        return;

    for (auto& node : mNodes) {
        for (size_t i = 0; i < node->numPorts(); i++) {
            IPort& port = node->getPortByIndex(i);
            if (port.getLinkID() == linkID) {
                port.unlink();
                return;
            }
        }
    }
}

void Graph::checkLinksDeleted() {
    const int numSelected = ImNodes::NumSelectedLinks();
    if (numSelected == 0 || !ImGui::IsKeyReleased(ImGuiKey_Delete))
        return;

    std::vector<int> selectedLinks(numSelected);
    ImNodes::GetSelectedLinks(selectedLinks.data());
    for (auto& node : mNodes) {
        for (size_t i = 0; i < node->numPorts(); i++) {
            IPort& port = node->getPortByIndex(i);
            if (std::find(selectedLinks.begin(), selectedLinks.end(), port.getLinkID()) != selectedLinks.end())
                port.unlink();
        }
    }
}

void Graph::checkNodesDeleted() {
    const int numSelected = ImNodes::NumSelectedNodes();
    if (numSelected == 0 || !ImGui::IsKeyReleased(ImGuiKey_Delete))
        return;

    std::vector<int> selectedNodes(numSelected);
    ImNodes::GetSelectedNodes(selectedNodes.data());
    mNodes.erase(
        std::remove_if(
            mNodes.begin(), mNodes.end(),
            [&selectedNodes](const auto& node) {
                return std::find(selectedNodes.begin(), selectedNodes.end(), node->getID()) != selectedNodes.end();
            }),
        mNodes.end()
    );
}
