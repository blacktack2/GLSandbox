#include "BasicNodes.h"

#include <imgui/imgui.h>

void IntegerNode::drawContents() {
    const std::string LABEL = std::string();
    ImGui::InputInt(LABEL.c_str(), &mValue);
}

void FloatNode::drawContents() {
    const std::string LABEL = std::string();
    ImGui::InputFloat(LABEL.c_str(), &mValue);
}
