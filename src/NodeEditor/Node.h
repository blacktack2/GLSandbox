#pragma once
#include "Ports.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ax::NodeEditor {
    struct EditorContext;
}

class Node {
public:
    virtual ~Node() = default;

    void draw();
    void drawLinks();

    [[nodiscard]] inline int getID() const {
        return mID;
    }

    [[nodiscard]] inline size_t numPorts() const {
        return mPorts.size();
    }

    [[nodiscard]] IPort& getPortByIndex(size_t i) {
        return mPorts[i].get();
    }

    [[nodiscard]] IPort* getPort(int portID) {
        for (auto& port : mPorts)
            if (port.get().getID() == portID)
                return &port.get();
        return nullptr;
    }
protected:
    explicit Node(std::string title, int id = 0);

    inline void addPort(IPort& port) {
        mPorts.emplace_back(port);
    }

    virtual void drawContents() = 0;
private:
    int mID;

    std::string mTitle;

    std::vector<std::reference_wrapper<IPort>> mPorts;
};

