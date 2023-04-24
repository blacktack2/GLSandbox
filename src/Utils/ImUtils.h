#pragma once
#include <imgui.h>

#include <filesystem>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

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

    void nodeDummy();

    void message(const char* format...);

    bool button(const std::string& displayText, const std::string& labelID);

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

    bool beginHeader(const std::string& displayText, const std::string& labelID, bool& show, unsigned int depth = 0);
    void endHeader();

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
        void circleIcon(float outerRadius, float thickness, ImU32 outerColour, ImU32 innerColour);
    }
};

