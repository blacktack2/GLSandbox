#pragma once
#include "Node.h"

#include <memory>
#include <vector>

struct ImNodesContext;

class Graph {
public:
    Graph();
    virtual ~Graph();

    void preDraw();
    void draw();
    void postDraw();
protected:
    virtual void drawNodeCreation() = 0;

    inline void addNode(std::unique_ptr<Node> node) {
        mNodes.push_back(std::move(node));
    }
private:
    void drawEditor();
    void drawConfig();

    ImNodesContext* mContext;

    std::vector<std::unique_ptr<Node>> mNodes{};

    bool mConfigPanelOpen = true;
};

