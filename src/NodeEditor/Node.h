#pragma once
#include "Ports.h"

#include <glm/vec2.hpp>

#include <functional>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ax::NodeEditor {
    struct EditorContext;
}

class Node {
public:
    virtual ~Node() = default;

    [[nodiscard]] virtual unsigned int getTypeID() = 0;

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn, std::unordered_map<int, std::reference_wrapper<OutPort>>& outPorts,
                     std::vector<std::pair<std::reference_wrapper<InPort>, int>>& links);

    void draw();
    void drawLinks();

    void setAbsolutePosition(const glm::vec2& pos);
    void setRelativePosition(const glm::vec2& pos);

    [[nodiscard]] glm::vec2 getAbsolutePositionC() const;
    [[nodiscard]] glm::vec2 getAbsolutePositionTR() const;
    [[nodiscard]] glm::vec2 getAbsolutePositionBR() const;
    [[nodiscard]] glm::vec2 getAbsolutePositionBL() const;
    [[nodiscard]] glm::vec2 getAbsolutePositionTL() const;

    [[nodiscard]] glm::vec2 getRelativePositionC() const;
    [[nodiscard]] glm::vec2 getRelativePositionTR() const;
    [[nodiscard]] glm::vec2 getRelativePositionBR() const;
    [[nodiscard]] glm::vec2 getRelativePositionBL() const;
    [[nodiscard]] glm::vec2 getRelativePositionTL() const;

    [[nodiscard]] glm::vec2 getSize() const;

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

    virtual void serializeContents(std::ofstream& streamOut) const = 0;
    virtual void deserializeContents(std::ifstream& streamIn) = 0;

    virtual void drawContents() = 0;

    inline void addPort(IPort& port) {
        mPorts.emplace_back(port);
        if (InPort* inPort = dynamic_cast<InPort*>(&port))
            mInPorts.emplace_back(*inPort);
        else if (OutPort* outPort = dynamic_cast<OutPort*>(&port))
            mOutPorts.emplace_back(*outPort);
        else
            assert(false); // Port must be an input or an output
    }
private:
    int mID;

    std::string mTitle;

    std::vector<std::reference_wrapper<IPort>> mPorts;
    std::vector<std::reference_wrapper<InPort>> mInPorts;
    std::vector<std::reference_wrapper<OutPort>> mOutPorts;
};

