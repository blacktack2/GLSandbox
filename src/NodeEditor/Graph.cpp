#include "Graph.h"

#include <imnodes.h>

Graph::Graph() {
    mContext = ImNodes::CreateContext();
    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
}

Graph::~Graph() {
    ImNodes::DestroyContext(mContext);
}

void Graph::serialize(std::ofstream& streamOut) const {
    streamOut << mNodes.size() << "\n";
    for (const auto& node : mNodes) {
        streamOut << node->getTypeID() << "\n";
        node->serialize(streamOut);
    }
}

void Graph::deserialize(std::ifstream& streamIn) {
    mNodes.clear();

    std::unordered_map<int, std::reference_wrapper<OutPort>> outPorts;
    std::vector<std::pair<std::reference_wrapper<InPort>, int>> links;

    size_t numNodes;
    streamIn >> numNodes;
    for (size_t i = 0; i < numNodes; i++) {
        unsigned int nodeType;
        streamIn >> nodeType;
        std::unique_ptr<Node> node = deserializeNodeType(nodeType);
        node->deserialize(streamIn, outPorts, links);
        addNode(std::move(node));
    }

    for (auto& link : links) {
        auto match = outPorts.find(link.second);
        if (match != outPorts.end())
            link.first.get().link(match->second);
    }
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

    OutPort* output = nullptr;
    InPort*  input  = nullptr;
    for (auto& node : mNodes) {
        output = output ? output : dynamic_cast<OutPort*>(node->getPort(outputID));
        input  = input  ? input  : dynamic_cast<InPort* >(node->getPort(inputID ));
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
