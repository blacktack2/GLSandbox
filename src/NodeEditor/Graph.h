#pragma once
#include "Node.h"

#include <memory>
#include <vector>

struct ImNodesContext;

class Graph {
public:
    Graph();
    virtual ~Graph();

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn);

    void preDraw();
    void draw();
    void postDraw();

    [[nodiscard]] inline size_t getNumNodes() const {
        return mNodes.size();
    }
protected:
    virtual std::unique_ptr<Node> deserializeNodeType(unsigned int nodeType) = 0;

    virtual void drawNodeCreation() = 0;

    inline void addNode(std::unique_ptr<Node> node) {
        mNodes.push_back(std::move(node));
    }
private:
    void drawEditor();
    void drawConfig();

    void checkLinkCreated();
    void checkLinkDestroyed();

    ImNodesContext* mContext;

    std::vector<std::unique_ptr<Node>> mNodes{};

    bool mConfigPanelOpen = true;
};

