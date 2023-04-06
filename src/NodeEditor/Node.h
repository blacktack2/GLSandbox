#pragma once
#include "Ports.h"

#include <glm/vec2.hpp>

#include <imgui.h>

#include <functional>
#include <fstream>
#include <map>
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
    void deserialize(std::ifstream& streamIn, std::streampos end,
                     std::unordered_map<int, std::reference_wrapper<OutPort>>& outPorts,
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
    enum class MessageType {
        Info,
        Warning,
        Error,
        Confirmation,
    };

    explicit Node(std::string title);

    [[nodiscard]] virtual std::vector<std::pair<std::string, std::string>> generateSerializedData() const = 0;
    virtual void deserializeData(const std::string& dataID, std::ifstream& stream) = 0;

    virtual void onDeserialize() {}

    virtual void drawContents() = 0;

    void addPort(IPort& port);
    void removePort(const IPort& port);

    static inline ImVec4 getMessageColour(MessageType type) {
        switch (type) {
            default:
            case MessageType::Info         : return ImVec4(1, 1, 1, 1);
            case MessageType::Warning      : return ImVec4(1, 1, 0, 1);
            case MessageType::Error        : return ImVec4(1, 0, 0, 1);
            case MessageType::Confirmation : return ImVec4(0, 1, 0, 1);
        }
    }
private:
    void writeDataPoints(std::ofstream& streamOut, char prefix,
                         const std::vector<std::pair<std::string, std::string>>& dataPoints) const;

    int mID;

    std::string mTitle;

    std::vector<std::reference_wrapper<IPort>> mPorts;
    std::vector<std::reference_wrapper<InPort>> mInPorts;
    std::vector<std::reference_wrapper<OutPort>> mOutPorts;
};

