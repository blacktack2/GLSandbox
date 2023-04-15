#pragma once
#include "../Utils/GraphUtils.h"

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

class IPort;

namespace ax::NodeEditor {
    struct EditorContext;
}

class Node {
public:
    typedef std::unordered_map<int, std::pair<Node*, std::string>> port_data_t;
    typedef std::unordered_map<Node*, std::vector<std::pair<std::string, int>>> link_data_t;

    virtual ~Node() = default;

    [[nodiscard]] virtual unsigned int getTypeID() = 0;

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn, std::streampos end,
                     port_data_t& staticPortData, link_data_t& staticLinkData,
                     port_data_t& dynamicPortData, link_data_t& dynamicLinkData);

    void draw();
    void drawLinks();

    void setAbsolutePosition(const glm::vec2& pos);

    [[nodiscard]] glm::vec2 getAbsolutePosition() const;

    [[nodiscard]] glm::vec2 getSize() const;

    [[nodiscard]] inline int getID() const {
        return mID;
    }

    [[nodiscard]] size_t numPorts() const;

    [[nodiscard]] IPort* getPortByName(const std::string& uniqueName);
    [[nodiscard]] IPort& getPortByIndex(size_t i);
    [[nodiscard]] IPort* getPort(int portID);
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

    template<typename... Args>
    inline std::string generateNodeLabel(const std::string& displayText, Args... args) const {
        return generateLabel(displayText, "Node", getID(), args...);
    }
    template<typename... Args>
    inline std::string generateNodePopupID(const std::string& popupName, Args... args) const {
        return generatePopupID(popupName, "Node", getID(), args...);
    }

    static inline void drawMessage(const std::string& text, const ImVec4& colour) {
        ImGui::PushStyleColor(ImGuiCol_Text, colour);
        ImGui::TextWrapped("%s", text.c_str());
        ImGui::PopStyleColor();
    }

    static inline constexpr float getNodeWidth() {
        return 200.0f;
    }
    static inline constexpr float getNodeMaxHeight() {
        return 200.0f;
    }
    static inline constexpr float getButtonWidth() {
        return 100.0f;
    }
    static inline constexpr ImVec2 getButtonBounds() {
        return ImVec2(getButtonWidth(), 0.0f);
    }
    static inline constexpr ImVec2 getChildPanelBounds() {
        return ImVec2(getNodeWidth() * 0.75, 100.0f);
    }
    static inline constexpr float getTextInputWidth() {
        return getNodeWidth() * 0.5f;
    }
    static inline constexpr float getNumericInputWidth() {
        return getNodeWidth() * 0.5f;
    }
    static inline constexpr float getMultiNumericInputWidth(unsigned int numComponents = 1) {
        return getNodeWidth() * 0.25f * (float)numComponents;
    }
    static inline constexpr float getComboWidth() {
        return getButtonWidth();
    }
    static inline constexpr float getComboItemWidth() {
        return getComboWidth();
    }
    static inline constexpr float getMiniComboWidth() {
        return 60.0f;
    }
    static inline constexpr float getMiniComboItemWidth() {
        return getMiniComboWidth();
    }
    static inline constexpr float getTreeNodeWidth() {
        return getNodeWidth();
    }
    static inline constexpr float getPopupSelectableWidth() {
        return getComboItemWidth();
    }

    static inline constexpr const char* getFloatFormat() {
        return "%.2f";
    }
    static inline constexpr const char* getIntFormat() {
        return "%d";
    }
private:
    int mID;

    std::string mTitle;

    std::vector<std::reference_wrapper<IPort>> mPorts;
    std::vector<std::reference_wrapper<IPort>> mInPorts;
    std::vector<std::reference_wrapper<IPort>> mOutPorts;

    glm::vec2 mPosition = glm::vec2(0.0f, 0.0f);
    bool mIsPositionDirty = false;
};
