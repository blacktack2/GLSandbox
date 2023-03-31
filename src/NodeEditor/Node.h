#pragma once
#include "Ports.h"

#include <glm/vec2.hpp>

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

    void setAbsolutePosition(const glm::vec2& pos);
    void setRelativePosition(const glm::vec2& pos);

    glm::vec2 getAbsolutePositionC() const;
    glm::vec2 getAbsolutePositionTR() const;
    glm::vec2 getAbsolutePositionBR() const;
    glm::vec2 getAbsolutePositionBL() const;
    glm::vec2 getAbsolutePositionTL() const;

    glm::vec2 getRelativePositionC() const;
    glm::vec2 getRelativePositionTR() const;
    glm::vec2 getRelativePositionBR() const;
    glm::vec2 getRelativePositionBL() const;
    glm::vec2 getRelativePositionTL() const;

    glm::vec2 getSize() const;

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

