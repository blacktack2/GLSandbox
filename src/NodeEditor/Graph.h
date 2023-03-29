#pragma once
#include "Node.h"

#include <memory>
#include <vector>

namespace ax::NodeEditor {
    struct EditorContext;
}

class Graph {
public:
    Graph();
    virtual ~Graph() = default;

    void draw();
protected:
    virtual void drawNodeCreation() = 0;

    inline void addNode(std::unique_ptr<Node> node) {
        mNodes.push_back(std::move(node));
    }
private:
    void drawEditor();
    void drawConfig();

    ax::NodeEditor::EditorContext* mContext;

    std::vector<std::unique_ptr<Node>> mNodes{};

    bool mConfigPanelOpen = true;
};

