#pragma once
#include "Node.h"

#include <functional>
#include <memory>
#include <vector>

namespace ed = ax::NodeEditor;

namespace ax::NodeEditor {
    struct EditorContext;
}

class Graph {
public:
    Graph();
    virtual ~Graph();

    virtual void initializeDefault() = 0;

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn);

    void preDraw();
    void draw();
    void postDraw();

    inline void clearNodes() {
        mNodes.clear();
    }

    [[nodiscard]] inline size_t getNumNodes() const {
        return mNodes.size();
    }

    void markDirty();
    void markClean();
    [[nodiscard]] inline bool isDirty() const {
        return mIsDirty;
    };

    inline void addOnDirtyEvent(std::function<void()> event) {
        mOnDirtyEvents.push_back(std::move(event));
    }
    inline void addOnCleanEvent(std::function<void()> event) {
        mOnCleanEvents.push_back(std::move(event));
    }
protected:
    virtual std::unique_ptr<Node> deserializeNodeType(const std::string& nodeType) = 0;
    virtual std::string getNodeSerialName(const Node& node) const = 0;

    virtual void drawNodeCreation() = 0;

    inline void addNode(std::unique_ptr<Node> node) {
        if (node) {
            node->setParent(this);
            mNodes.push_back(std::move(node));
            markDirty();
        }
    }
private:
    void drawEditor();
    void drawConfig();

    void checkLinkCreated();
    void checkLinksDeleted();
    void checkNodesDeleted();

    ed::EditorContext* mContext;

    std::vector<std::unique_ptr<Node>> mNodes{};

    bool mConfigPanelOpen = true;

    bool mIsDirty = false;
    std::vector<std::function<void()>> mOnDirtyEvents{};
    std::vector<std::function<void()>> mOnCleanEvents{};
};

