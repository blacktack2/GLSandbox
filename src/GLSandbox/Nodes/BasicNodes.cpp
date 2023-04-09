#include "BasicNodes.h"

#include <imgui.h>

IntegerNode::IntegerNode() : NumericNode<int>("Integer") {

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

bool IntegerNode::drawInputArea(const std::string& label) {
    return ImGui::InputInt(label.c_str(), &mValue);
}

FloatNode::FloatNode() : NumericNode<float>("Float") {

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

bool FloatNode::drawInputArea(const std::string& label) {
    return ImGui::InputFloat(label.c_str(), &mValue);
}

Vec2Node::Vec2Node() : NumericNode<glm::vec2>("Vec2") {

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

bool Vec2Node::drawInputArea(const std::string& label) {
    return ImGui::InputFloat2(label.c_str(), &mValue[0]);
}

Vec3Node::Vec3Node() : NumericNode<glm::vec3>("Vec3") {

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

bool Vec3Node::drawInputArea(const std::string& label) {
    return ImGui::InputFloat3(label.c_str(), &mValue[0]);
}

Vec4Node::Vec4Node() : NumericNode<glm::vec4>("Vec4") {

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

bool Vec4Node::drawInputArea(const std::string& label) {
    return ImGui::InputFloat4(label.c_str(), &mValue[0]);
}
