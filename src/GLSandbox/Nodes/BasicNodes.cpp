#include "BasicNodes.h"

#include "../../NodeEditor/Ports.h"

#include <imgui.h>

IntegerNode::IntegerNode() : Node("Integer") {
    addPort(mValueOut);
}

std::vector<std::pair<std::string, std::string>> IntegerNode::generateSerializedData() const {
    return {
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
    addPort(mValueOut);
}

std::vector<std::pair<std::string, std::string>> FloatNode::generateSerializedData() const {
    return {
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

Vec2Node::Vec2Node() : Node("Vec2") {
    addPort(mValueOut);
}

std::vector<std::pair<std::string, std::string>> Vec2Node::generateSerializedData() const {
    return {
        {"Value", std::to_string(mValue.x).append(" ").append(std::to_string(mValue.y))},
    };
}

void Vec2Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value") {
        stream >> mValue.x;
        stream >> mValue.y;
    }
}

void Vec2Node::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputFloat2(LABEL.c_str(), &mValue[0]);
}

Vec3Node::Vec3Node() : Node("Vec3") {
    addPort(mValueOut);
}

std::vector<std::pair<std::string, std::string>> Vec3Node::generateSerializedData() const {
    return {
        {"Value", std::to_string(mValue.x).append(" ").append(std::to_string(mValue.y))
                  .append(" ").append(std::to_string(mValue.z))},
    };
}

void Vec3Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value") {
        stream >> mValue.x;
        stream >> mValue.y;
        stream >> mValue.z;
    }
}

void Vec3Node::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputFloat3(LABEL.c_str(), &mValue[0]);
}

Vec4Node::Vec4Node() : Node("Vec4") {
    addPort(mValueOut);
}

std::vector<std::pair<std::string, std::string>> Vec4Node::generateSerializedData() const {
    return {
        {"Value", std::to_string(mValue.x).append(" ").append(std::to_string(mValue.y))
                  .append(" ").append(std::to_string(mValue.z)).append(" ").append(std::to_string(mValue.w))},
    };
}

void Vec4Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value") {
        stream >> mValue.x;
        stream >> mValue.y;
        stream >> mValue.z;
        stream >> mValue.w;
    }
}

void Vec4Node::drawContents() {
    const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
    ImGui::InputFloat4(LABEL.c_str(), &mValue[0]);
}
