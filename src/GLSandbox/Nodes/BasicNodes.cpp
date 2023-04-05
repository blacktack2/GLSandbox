#include "BasicNodes.h"

#include <imgui.h>

IntegerNode::IntegerNode() : Node("Integer") {
    addPort(mIntOut);
}

std::map<std::string, std::string> IntegerNode::generateSerializedData() const {
    return std::map<std::string, std::string>{
        {"Value", std::to_string(mValue)},
    };
}

void IntegerNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue;
}

void IntegerNode::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputInt(LABEL.c_str(), &mValue);
}

FloatNode::FloatNode() : Node("Float") {
    addPort(mFloatOut);
}

std::map<std::string, std::string> FloatNode::generateSerializedData() const {
    return std::map<std::string, std::string>{
        {"Value", std::to_string(mValue)},
    };
}

void FloatNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue;
}

void FloatNode::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputFloat(LABEL.c_str(), &mValue);
}
