#include "BasicNodes.h"

#include <imgui.h>

void IntegerNode::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputInt(LABEL.c_str(), &mValue);
}

void FloatNode::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputFloat(LABEL.c_str(), &mValue);
}
