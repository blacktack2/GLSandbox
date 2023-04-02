#include "BasicNodes.h"

#include <imgui.h>

IntegerNode::IntegerNode() : Node("Integer") {
    addPort(mIntOut);
}

void IntegerNode::serializeContents(std::ofstream& streamOut) const {
    streamOut << mValue;
}

void IntegerNode::deserializeContents(std::ifstream& streamIn) {
    streamIn >> mValue;
}

void IntegerNode::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputInt(LABEL.c_str(), &mValue);
}

FloatNode::FloatNode() : Node("Float") {
    addPort(mFloatOut);
}

void FloatNode::serializeContents(std::ofstream& streamOut) const {
    streamOut << mValue;
}

void FloatNode::deserializeContents(std::ifstream& streamIn) {
    streamIn >> mValue;
}

void FloatNode::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputFloat(LABEL.c_str(), &mValue);
}
