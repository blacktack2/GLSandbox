#include "ImUtils.h"

#include "../Rendering/Texture.h"

#include "FileUtils.h"

#include <imgui_stdlib.h>
#include <imgui_node_editor.h>

namespace ed = ax::NodeEditor;

static constexpr float gNODE_WIDTH = 200.0f;

static constexpr ImVec2 gBUTTON_BOUNDS = ImVec2(gNODE_WIDTH * 0.75f, 0.0f);

static constexpr ImVec2 gHEADER_BOUNDS = ImVec2(gNODE_WIDTH * 0.75f, 0.0f);
static constexpr ImVec4 gHEADER_COLOURS[] = {
    ImVec4(0.25f, 0.25f, 0.25f, 1.0f),
    ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
};

static constexpr ImVec2 gCHILD_PANEL_BOUNDS = ImVec2(gNODE_WIDTH * 0.75f, 0.0f);

static constexpr float gTEXT_INPUT_WIDTH = gNODE_WIDTH * 0.5f;

static constexpr float gNUMERIC_INPUT_WIDTH = gNODE_WIDTH * 0.5f;
static constexpr float gMULTI_NUMERIC_INPUT_WIDTH = gNODE_WIDTH * 0.25f;
static constexpr ImVec2 gMULTI_BUTTON_INPUT_BOUNDS = ImVec2(gMULTI_NUMERIC_INPUT_WIDTH, 0.0f);

static constexpr ImVec2 gCYCLE_BUTTON_NOTCH_BOUNDS = ImVec2(30.0f, 0.0f);
static constexpr float gCYCLE_BUTTON_WIDTH = gBUTTON_BOUNDS.x - gCYCLE_BUTTON_NOTCH_BOUNDS.x * 2.0f;

static constexpr ImVec2 gRANGE_BUTTON_NOTCH_BOUNDS = ImVec2(30.0f, 0.0f);

static constexpr float gFILE_TEXT_WIDTH = gNODE_WIDTH * 0.5f;
static constexpr ImVec2 gFILE_BUTTON_BOUNDS = ImVec2(gNODE_WIDTH * 0.25f, 0.0f);

static constexpr float gIMAGE_WIDTH = gNODE_WIDTH;

static constexpr const char* gFLOAT_FORMAT = "%.2f";
static constexpr const char* gINT_FORMAT = "%d";

static constexpr float gPIN_ICON_THICKNESS = 2.0f;

std::string formatLabel(std::string display, const std::string& id) {
    return display.append("##").append(id);
}
std::string formatLabel(const std::string& id) {
    return std::string("##").append(id);
}

void ImUtils::nodeDummy() {
    ImGui::Dummy(ImVec2(gNODE_WIDTH, 0.0f));
}

bool ImUtils::inputNodeName(std::string& name, const std::string& labelID, bool isDirty) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::SetNextItemWidth(ImGui::CalcTextSize(name.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f);
    bool changed = ImGui::InputText(formatLabel(labelID).c_str(), &name);

    if (isDirty) {
        ImGui::SameLine();
        ImGui::TextUnformatted("*");
    }

    ImGui::PopStyleColor();

    return changed;
}

void ImUtils::message(const char* format...) {
    va_list args;
    va_start(args, format);
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + gNODE_WIDTH);
    ImGui::Text(format, args);
    ImGui::PopTextWrapPos();
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

bool ImUtils::inputBoolN(bool* value, unsigned int numComponents, const std::string& labelID) {
    bool valueChanged = false;
    for (unsigned int i = 0; i < numComponents; i++) {
        if (i != 0)
            ImGui::SameLine(gMULTI_NUMERIC_INPUT_WIDTH * (float)i);
        if (ImGui::Button(formatLabel(value[i] ? "X" : "O", labelID + std::to_string(i)).c_str(), gMULTI_BUTTON_INPUT_BOUNDS)) {
            valueChanged = true;
            value[i] = !value[i];
        }
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

bool ImUtils::beginHeader(const std::string& displayText, const std::string& labelID, bool& show, unsigned int depth) {
    ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(gNODE_WIDTH - gHEADER_BOUNDS.x, 0.0f) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Button, gHEADER_COLOURS[depth]);
    if (ImGui::Button(formatLabel(displayText, labelID).c_str(), gHEADER_BOUNDS))
        show = !show;
    ImGui::PopStyleColor();
    return show;
}

void ImUtils::endHeader() {

}

bool ImUtils::toggleButton(bool& value, const std::string& displayOnEnable, const std::string& displayOnDisable,
                           const std::string& labelID) {
    if (ImGui::Button(formatLabel(value ? displayOnEnable : displayOnDisable, labelID).c_str(), gBUTTON_BOUNDS)) {
        value = !value;
        return true;
    }
    return false;
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

bool ImUtils::fileChooseDialog(std::filesystem::path& filepath, const std::filesystem::path& defaultLocation,
                               const std::string& labelID, const std::vector<std::string>& filters) {
    if (ImGui::Button(formatLabel("Choose", labelID).c_str(), gFILE_BUTTON_BOUNDS))
        return FileUtils::openFileDialog(filepath, defaultLocation, filters);

    ImGui::SameLine();

    const std::string displayName = filepath.filename();
    ImGui::Text("%s", filepath.empty() ? "<none>" : displayName.c_str());
    return false;
}

void ImUtils::image(const Texture& tex, const std::string& labelID, bool& show) {
    if (!beginHeader("Preview", labelID, show))
        return;

    float height = gIMAGE_WIDTH * (float)tex.getHeight() / (float)tex.getWidth();
    ImGui::Image((ImTextureID)(intptr_t)tex.getID(), ImVec2(gIMAGE_WIDTH, height));

    endHeader();
}

struct DummyData {
    ImVec2 min;
    ImVec2 max;
    ImVec2 size;
    ImVec2 center;
};

void drawPinDummy(float width, float height, DummyData& data) {
    float lineHeight = ImGui::GetTextLineHeight();
    float yOffset = (lineHeight - height * 2.0f);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(cursorPos + ImVec2(0.0f, yOffset));

    ImGui::Dummy(ImVec2(width, height) * 2.0f);

    ImGui::SetCursorScreenPos(cursorPos + ImVec2(width, 0.0f));

    data.min = ImGui::GetItemRectMin() + ImVec2(gPIN_ICON_THICKNESS, gPIN_ICON_THICKNESS) * 0.5f;
    data.max = ImGui::GetItemRectMax() - ImVec2(gPIN_ICON_THICKNESS, gPIN_ICON_THICKNESS) * 0.5f;
    data.size = ImGui::GetItemRectSize() + ImVec2(gPIN_ICON_THICKNESS, gPIN_ICON_THICKNESS);
    data.center = (data.min + data.max) * 0.5f;
}

void ImUtils::Pins::circleIcon(float outerRadius, ImU32 outerColour, ImU32 innerColour) {
    auto drawList = ImGui::GetWindowDrawList();

    DummyData data;
    drawPinDummy(outerRadius, outerRadius, data);
    float radius = std::min(data.size.x, data.size.y) * 0.5f;

    drawList->AddCircleFilled(data.center, radius - gPIN_ICON_THICKNESS * 0.5f, innerColour, 24);
    drawList->AddCircle(data.center, radius, outerColour, 24, gPIN_ICON_THICKNESS);
}

void ImUtils::Pins::triangleIcon(float size, ImU32 outerColour, ImU32 innerColour) {
    auto drawList = ImGui::GetWindowDrawList();

    DummyData data;
    drawPinDummy(size, size, data);

    drawList->AddTriangleFilled(data.min, ImVec2(data.min.x, data.max.y), ImVec2(data.max.x, data.center.y), innerColour);
    drawList->AddTriangle(data.min, ImVec2(data.min.x, data.max.y), ImVec2(data.max.x, data.center.y), outerColour, gPIN_ICON_THICKNESS);
}

void ImUtils::Pins::squareIcon(float size, ImU32 outerColour, ImU32 innerColour) {
    auto drawList = ImGui::GetWindowDrawList();

    DummyData data;
    drawPinDummy(size, size, data);

    drawList->AddRectFilled(data.min, data.max, innerColour);
    drawList->AddRect(data.min, data.max, innerColour, 0.0f, ImDrawFlags_None, gPIN_ICON_THICKNESS);
}

void ImUtils::Pins::arrowIcon(float size, ImU32 outerColour, ImU32 innerColour) {
    auto drawList = ImGui::GetWindowDrawList();

    DummyData data;
    drawPinDummy(size, size, data);

    float offsetX = data.min.x + (data.max.x - data.min.x) * 0.3f;

    ImVec2 points[] = {
        data.min,
        {offsetX, data.min.y},
        {data.max.x, data.center.y},
        {offsetX, data.max.y},
        {data.min.x, data.max.y},
    };
    drawList->AddConvexPolyFilled(points, 5, innerColour);
    drawList->AddPolyline(points, 5, outerColour, ImDrawFlags_Closed, gPIN_ICON_THICKNESS);
}
