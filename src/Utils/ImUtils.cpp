#include "ImUtils.h"

#include "FileUtils.h"

#include <imgui_stdlib.h>

static constexpr float gNODE_WIDTH = 200.0f;

static constexpr ImVec2 gBUTTON_BOUNDS = ImVec2(100.0f, 0.0f);

static constexpr ImVec2 gCHILD_PANEL_BOUNDS = ImVec2(gNODE_WIDTH * 0.75f, 0.0f);

static constexpr float gTEXT_INPUT_WIDTH = gNODE_WIDTH * 0.5f;

static constexpr float gNUMERIC_INPUT_WIDTH = gNODE_WIDTH * 0.5f;
static constexpr float gMULTI_NUMERIC_INPUT_WIDTH = gNODE_WIDTH * 0.25f;

static constexpr ImVec2 gCYCLE_BUTTON_NOTCH_BOUNDS = ImVec2(30.0f, 0.0f);
static constexpr float gCYCLE_BUTTON_WIDTH = gBUTTON_BOUNDS.x - gCYCLE_BUTTON_NOTCH_BOUNDS.x * 2.0f;

static constexpr ImVec2 gRANGE_BUTTON_NOTCH_BOUNDS = ImVec2(30.0f, 0.0f);

static constexpr float gFILE_TEXT_WIDTH = gNODE_WIDTH * 0.5f;
static constexpr ImVec2 gFILE_BUTTON_BOUNDS = ImVec2(gNODE_WIDTH * 0.25f, 0.0f);

static constexpr const char* gFLOAT_FORMAT = "%.2f";
static constexpr const char* gINT_FORMAT = "%d";

std::string formatLabel(std::string display, const std::string& id) {
    return display.append("##").append(id);
}
std::string formatLabel(const std::string& id) {
    return std::string("##").append(id);
}

void ImUtils::nodeDummy() {
    ImGui::Dummy(ImVec2(gNODE_WIDTH, 0.0f));
}

void ImUtils::message(const char* format...) {
    va_list args;
    va_start(args, format);
    ImGui::TextWrapped(format, args);
    va_end(args);
}

bool ImUtils::button(const std::string& displayText, const std::string& labelID) {
    return ImGui::Button(formatLabel(displayText, labelID).c_str(), gBUTTON_BOUNDS);
}

bool ImUtils::inputText(std::string& text, const std::string& labelID) {
    ImGui::SetNextItemWidth(gTEXT_INPUT_WIDTH);
    return ImGui::InputText(formatLabel(labelID).c_str(), &text);
}

bool ImUtils::inputFloat(float* value, const std::string& labelID, float min, float max) {
    ImGui::SetNextItemWidth(gNUMERIC_INPUT_WIDTH);
    return ImGui::DragFloat(formatLabel(labelID).c_str(), value, 0.1f, min, max, gFLOAT_FORMAT);
}

bool ImUtils::inputFloatN(float* value, unsigned int numComponents, const std::string& labelID,
                          float min, float max) {
    ImGui::SetNextItemWidth(gMULTI_NUMERIC_INPUT_WIDTH * (float)numComponents);
    switch (numComponents) {
        default:
        case 1: return ImGui::DragFloat(formatLabel(labelID).c_str(), value, 0.1f, min, max, gFLOAT_FORMAT);
        case 2: return ImGui::DragFloat2(formatLabel(labelID).c_str(), value, 0.1f, min, max, gFLOAT_FORMAT);
        case 3: return ImGui::DragFloat3(formatLabel(labelID).c_str(), value, 0.1f, min, max, gFLOAT_FORMAT);
        case 4: return ImGui::DragFloat4(formatLabel(labelID).c_str(), value, 0.1f, min, max, gFLOAT_FORMAT);
    }
}

bool ImUtils::inputFloatNxN(float* value, unsigned int numComponents, const std::string& labelID,
                            float min, float max) {
    bool valueChanged = false;
    for (unsigned int i = 0; i < numComponents; i++) {
        valueChanged |= inputFloatN(&value[i * numComponents], numComponents, labelID + std::to_string(i), min, max);
    }
    return valueChanged;
}

bool ImUtils::inputInt(int* value, const std::string& labelID, int min, int max) {
    ImGui::SetNextItemWidth(gNUMERIC_INPUT_WIDTH);
    return ImGui::DragInt(formatLabel(labelID).c_str(), value, 1, min, max, gINT_FORMAT);
}

bool ImUtils::inputIntN(int* value, unsigned int numComponents, const std::string& labelID,
                        int min, int max) {
    ImGui::SetNextItemWidth(gMULTI_NUMERIC_INPUT_WIDTH * (float)numComponents);
    switch (numComponents) {
        default:
        case 1: return ImGui::DragInt(formatLabel(labelID).c_str(), value, 1, min, max, gINT_FORMAT);
        case 2: return ImGui::DragInt2(formatLabel(labelID).c_str(), value, 1, min, max, gINT_FORMAT);
        case 3: return ImGui::DragInt3(formatLabel(labelID).c_str(), value, 1, min, max, gINT_FORMAT);
        case 4: return ImGui::DragInt4(formatLabel(labelID).c_str(), value, 1, min, max, gINT_FORMAT);
    }
}

bool ImUtils::inputIntNxN(int* value, unsigned int numComponents, const std::string& labelID,
                          int min, int max) {
    bool valueChanged = false;
    for (unsigned int i = 0; i < numComponents; i++) {
        valueChanged |= inputIntN(&value[i * numComponents], numComponents, labelID + std::to_string(i), min, max);
    }
    return valueChanged;
}

void ImUtils::multiInputLabel(const std::string& label0) {
    ImGui::Text("%s", label0.c_str());
}

void ImUtils::multiInputLabel(const std::string& label0, const std::string& label1) {
    ImGui::Text("%s", label0.c_str());
    ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH);
    ImGui::Text("%s", label1.c_str());
}

void ImUtils::multiInputLabel(const std::string& label0, const std::string& label1,
                              const std::string& label2) {
    ImGui::Text("%s", label0.c_str());
    ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH);
    ImGui::Text("%s", label1.c_str());
    ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH * 2.0f);
    ImGui::Text("%s", label2.c_str());
}

void ImUtils::multiInputLabel(const std::string& label0, const std::string& label1,
                              const std::string& label2, const std::string& label3) {
    ImGui::Text("%s", label0.c_str());
    ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH);
    ImGui::Text("%s", label1.c_str());
    ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH * 2.0f);
    ImGui::Text("%s", label2.c_str());
    ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH * 3.0f);
    ImGui::Text("%s", label3.c_str());
}

bool ImUtils::beginHeader(const std::string& displayText, const std::string& labelID) {
    return ImGui::TreeNodeEx(formatLabel(displayText, labelID).c_str());
}

void ImUtils::endHeader() {
    ImGui::TreePop();
}

bool ImUtils::cycleButton(const std::string& labelID, size_t& index,
                          const std::vector<std::string>& contents) {
    bool valueChanged = false;
    size_t newIndex = index;
    if (ImGui::Button(formatLabel("<", labelID).c_str(), gCYCLE_BUTTON_NOTCH_BOUNDS)) {
        newIndex = (index == 0) ? contents.size() - 1 : index - 1;
        valueChanged = true;
    }

    ImGui::SetNextItemWidth(gCYCLE_BUTTON_WIDTH);
    ImGui::Text("%s", contents[index].c_str());

    if (ImGui::Button(formatLabel(">", labelID).c_str(), gCYCLE_BUTTON_NOTCH_BOUNDS)) {
        newIndex = (index >= contents.size() - 1) ? 0 : index + 1;
        valueChanged = true;
    }
    index = newIndex;
    return valueChanged;
}

bool ImUtils::cycleButton(const std::string& labelID, size_t& index,
                          size_t max, const std::function<std::string(size_t)>& contents) {
    bool valueChanged = false;
    size_t newIndex = index;
    if (ImGui::Button(formatLabel("<", labelID + "Left").c_str(), gCYCLE_BUTTON_NOTCH_BOUNDS)) {
        newIndex = (index == 0) ? max - 1 : index - 1;
        valueChanged = true;
    }

    ImGui::SameLine();

    ImGui::SetNextItemWidth(gCYCLE_BUTTON_WIDTH);
    ImGui::Text("%s", contents(index).c_str());

    ImGui::SameLine();

    if (ImGui::Button(formatLabel(">", labelID + "Right").c_str(), gCYCLE_BUTTON_NOTCH_BOUNDS)) {
        newIndex = (index >= max - 1) ? 0 : index + 1;
        valueChanged = true;
    }
    index = newIndex;
    return valueChanged;
}

bool ImUtils::rangeButton(const std::string& labelID, size_t& numSelected, size_t max) {
    bool valueChanged = false;
    size_t changedIndex = 0;

    if (ImGui::Button(formatLabel(0 < numSelected ? "X" : "O", labelID).c_str(), gRANGE_BUTTON_NOTCH_BOUNDS))
        valueChanged = true;
    for (size_t i = 1; i < max; i++) {
        ImGui::SameLine();
        if (ImGui::Button(formatLabel(i < numSelected ? "X" : "O", labelID + std::to_string(i)).c_str(), gRANGE_BUTTON_NOTCH_BOUNDS)) {
            valueChanged = true;
            changedIndex = i;
        }
    }

    if (valueChanged)
        numSelected = numSelected == (changedIndex + 1) ? changedIndex : changedIndex + 1;
    return valueChanged;
}

void ImUtils::rangeButtonLabel(const std::vector<std::string>& labels) {
    ImGui::Text("%s", labels[0].c_str());
    for (size_t i = 1; i < labels.size(); i++) {
        ImGui::SameLine((float)(i + 1) * gRANGE_BUTTON_NOTCH_BOUNDS.x);
        ImGui::Text("%s", labels[i].c_str());
    }
}

bool ImUtils::fileChooseDialog(std::string& filename, const std::string& labelID) {
    if (ImGui::Button(formatLabel("Choose", labelID).c_str(), gFILE_BUTTON_BOUNDS))
        return FileUtils::openFileDialog(filename);

    ImGui::SameLine();

    std::size_t index = filename.find_last_of('/');
    ImGui::Text("%s", filename.empty() ? "<none>" : filename.substr(index == std::string::npos ? 0 : index + 1).c_str());
    return false;
}
