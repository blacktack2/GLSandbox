#pragma once
#include <imgui.h>

#include <filesystem>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

class Node;
class Texture;

namespace ImUtils {
    template<typename T>
    void concatString(std::stringstream& stream, const std::string& delimiter, T val) {
        stream << val;
    }

    template<typename... Args, typename T>
    void concatString(std::stringstream& stream, const std::string& delimiter, T val, Args... args) {
        stream << val << delimiter;
        concatString(stream, delimiter, args...);
    }

    template<typename... Args>
    std::string generateLabelID(Args... args) {
        std::stringstream stream;
        concatString(stream, "_", args...);
        return stream.str();
    }

    void begin();
    void end();

    bool beginHeader(const std::string& displayText, const std::string& labelID, bool& show, unsigned int depth = 0);
    void endHeader();

    void setDataPanel(const std::string& labelID, const Node& node, std::function<void()> panelCallback);
    void softUnsetDataPanel(const Node& node);
    void drawDataPanel();

    void postTooltip(std::function<void()> contentsCallback, const ImVec2& position = ImVec2(0.0f, 0.0f));

    void nodeDummy();
    bool inputNodeName(std::string& name, const std::string& labelID, bool isDirty);

    void message(const char* format...);

    bool button(const std::string& displayText, const std::string& labelID);

    void dataPanelButton(const std::string& displayText, const std::string& labelID,
                         const Node& node, std::function<void()> panelCallback);

    bool inputText(std::string& text, const std::string& labelID);

    bool inputFloat(float* value, const std::string& labelID, float min = 0.0f, float max = 0.0f);
    bool inputFloatN(float* value, unsigned int numComponents, const std::string& labelID,
                            float min = 0.0f, float max = 0.0f);
    bool inputFloatNxN(float* value, unsigned int numComponents, const std::string& labelID,
                              float min = 0.0f, float max = 0.0f);
    bool inputInt(int* value, const std::string& labelID, int min = 0, int max = 0);
    bool inputIntN(int* value, unsigned int numComponents, const std::string& labelID,
                          int min = 0, int max = 0);
    bool inputIntNxN(int* value, unsigned int numComponents, const std::string& labelID,
                            int min = 0, int max = 0);

    bool inputBoolN(bool* value, unsigned int numComponents, const std::string& labelID);

    void multiInputLabel(const std::string& label0);
    void multiInputLabel(const std::string& label0, const std::string& label1);
    void multiInputLabel(const std::string& label0, const std::string& label1,
                                const std::string& label2);
    void multiInputLabel(const std::string& label0, const std::string& label1,
                                const std::string& label2, const std::string& label3);

    bool toggleButton(bool& value, const std::string& displayOnEnable, const std::string& displayOnDisable,
                      const std::string& labelID);

    bool cycleButton(const std::string& labelID, size_t& index,
                            const std::vector<std::string>& contents);
    bool cycleButton(const std::string& labelID, size_t& index,
                            size_t max, const std::function<std::string(size_t)>& contents);

    bool rangeButton(const std::string& labelID, size_t& numSelected, size_t max);
    void rangeButtonLabel(const std::vector<std::string>& labels);

    bool fileChooseDialog(std::filesystem::path& filepath, const std::filesystem::path& defaultLocation,
                          const std::string& labelID, const std::vector<std::string>& filters);

    void image(const Texture& tex, const std::string& labelID, bool& show);

    namespace Pins {
        void circleIcon(float radius, ImU32 outerColour, ImU32 innerColour);
        void nGonIcon(float size, int numSegments, ImU32 outerColour, ImU32 innerColour);
        void squareIcon(float size, ImU32 outerColour, ImU32 innerColour);
        void arrowIcon(float size, ImU32 outerColour, ImU32 innerColour);
    }
};

