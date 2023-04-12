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

extern int gNodeIDCounter;

class IPort;

namespace ax::NodeEditor {
    struct EditorContext;
}

class Node {
public:
    virtual ~Node() = default;

    [[nodiscard]] virtual unsigned int getTypeID() = 0;

    void serialize(std::ofstream& streamOut) const;
    void deserialize(std::ifstream& streamIn, std::streampos end,
                     std::unordered_map<int, std::reference_wrapper<IPort>>& outPorts,
                     std::vector<std::pair<std::reference_wrapper<IPort>, int>>& links);

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

    [[nodiscard]] size_t numPorts() const;

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

    inline void drawMessage(const std::string& text, const ImVec4& colour) {
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
        return getNodeWidth() * 0.25f * numComponents;
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
    void writeDataPoints(std::ofstream& streamOut, char prefix,
                         const std::vector<std::pair<std::string, std::string>>& dataPoints) const;

    int mID;

    std::string mTitle;

    std::vector<std::reference_wrapper<IPort>> mPorts;
    std::vector<std::reference_wrapper<IPort>> mInPorts;
    std::vector<std::reference_wrapper<IPort>> mOutPorts;
};
