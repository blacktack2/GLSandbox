#pragma once
#include "Node.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

namespace ed = ax::NodeEditor;

namespace ax::NodeEditor {
    struct EditorContext;
}

class Graph {
public:
    Graph();
    virtual ~Graph();

    void safeDestroy();

    void initializeDefault();

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn);

    void preDraw();
    void draw();
    void postDraw();

    inline void clearNodes() {
        mNodes.clear();
        onClear();
    }

    [[nodiscard]] inline size_t getNumNodes() const {
        return mNodes.size();
    }

    void regenerateID();
    [[nodiscard]] inline long getID() const {
        return mID;
    }

    inline void addDependency(long dependency) {
        mDependencies.emplace(dependency);
    }
    inline void removeDependency(long dependency) {
        mDependencies.erase(dependency);
    }
    [[nodiscard]] inline bool isDependency(long dependency) const {
        return mDependencies.contains(dependency);
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
    virtual void onDefaultInitialize() = 0;

    virtual std::unique_ptr<Node> deserializeNodeType(const std::string& nodeType) = 0;
    [[nodiscard]] virtual std::string getNodeSerialName(const Node& node) const = 0;

    virtual void drawNodeCreation() = 0;
    virtual void drawInputPanelContents() = 0;
    virtual void drawOutputPanelContents() = 0;

    virtual void onClear() = 0;
    virtual void onNodeAdd(Node& node) = 0;
    virtual void onNodeDelete(Node& node) = 0;

    void addNode(std::unique_ptr<Node> node);
private:
    void drawEditor();
    void drawConfig();
    void drawInputPanel();
    void drawOutputPanel();

    void checkLinkCreated();
    void checkLinksDeleted();
    void checkNodesDeleted();

    ed::EditorContext* mContext;

    long long mID = 0;

    std::vector<std::unique_ptr<Node>> mNodes{};

    std::unordered_set<int> mDependencies;

    bool mConfigPanelOpen = true;

    bool mIsDirty = false;
    std::vector<std::function<void()>> mOnDirtyEvents{};
    std::vector<std::function<void()>> mOnCleanEvents{};
};

