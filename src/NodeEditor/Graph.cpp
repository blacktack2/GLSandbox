#include "Graph.h"

#include <imgui/imnodes.h>

Graph::Graph() {

}

void Graph::draw() {
    ImNodes::BeginNodeEditor();

    for (auto& node : mNodes)
        node->draw();

    ImNodes::EndNodeEditor();
}
