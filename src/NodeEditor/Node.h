#pragma once
#include "../Utils/ImUtils.h"

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

extern int gGraphIDCounter;

class Graph;
class IPort;

namespace ax::NodeEditor {
    struct EditorContext;
}

class Node {
public:
    typedef std::unordered_map<int, std::pair<Node*, std::string>> port_data_t;
    typedef std::unordered_map<Node*, std::vector<std::pair<std::string, int>>> link_data_t;

    virtual ~Node() = default;

    void setParent(Graph* parent);

    [[nodiscard]] virtual unsigned int getTypeID() = 0;

    [[nodiscard]] virtual bool validate() const {
        return true;
    }

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn, std::streampos end,
                     port_data_t& staticPortData, link_data_t& staticLinkData,
                     port_data_t& dynamicPortData, link_data_t& dynamicLinkData);

    void draw();
    void drawLinks();

    void setAbsolutePosition(const ImVec2& pos);
    [[nodiscard]] ImVec2 getAbsolutePosition() const;

    [[nodiscard]] ImVec2 getRelativePosition() const;
    void setRelativePosition(const ImVec2& pos);

    [[nodiscard]] ImVec2 getSize() const;

    [[nodiscard]] inline int getID() const {
        return mID;
    }

    [[nodiscard]] size_t numPorts() const;

    [[nodiscard]] IPort* getPortByName(const std::string& uniqueName);
    [[nodiscard]] IPort& getPortByIndex(size_t i);
    [[nodiscard]] IPort* getPort(int portID);

    void markDirty();
    void markClean();
    [[nodiscard]] inline bool isDirty() const {
        return mIsDirty;
    }

    void lock();
    void unlock();
    [[nodiscard]] inline bool isLocked() const {
        return mIsLocked;
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

    virtual void onLock() {}
    virtual void onUnlock() {}

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

    template<typename... Args>
    inline std::string generateNodeLabelID(const std::string& label, Args... args) const {
        return ImUtils::generateLabelID("Node", getID(), label, args...);
    }

    static inline void drawMessage(const std::string& text, const ImVec4& colour) {
        ImGui::PushStyleColor(ImGuiCol_Text, colour);
        ImUtils::message(text.c_str());
        ImGui::PopStyleColor();
    }
private:
    Graph* mParent = nullptr;

    int mID;

    std::string mName;

    std::vector<std::reference_wrapper<IPort>> mPorts;
    std::vector<std::reference_wrapper<IPort>> mInPorts;
    std::vector<std::reference_wrapper<IPort>> mOutPorts;

    bool mIsDirty = true;

    ImVec2 mPosition = ImVec2(0.0f, 0.0f);
    bool mIsPositionDirty = false;
    bool mIsPositionRelative = false;

    bool mIsLocked = false;
};
