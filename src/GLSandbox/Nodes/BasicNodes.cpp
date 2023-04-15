#include "BasicNodes.h"

#include <sstream>

IntegerNode::IntegerNode() : NumericNode<int>("Integer") {
    mValue = 0;
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
    return ImUtils::inputIntN(&mValue, 1, label);
}

FloatNode::FloatNode() : NumericNode<float>("Float") {
    mValue = 0.0f;
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
    return ImUtils::inputFloatN(&mValue, 1, label);
}

Vec2Node::Vec2Node() : NumericNode<glm::vec2>("Vec2") {
    mValue = glm::vec2(0.0f);
}

std::vector<std::pair<std::string, std::string>> Vec2Node::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue.x << " " << mValue.y;
    return {
        {"Value", stream.str()}
    };
}

void Vec2Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue.x >> mValue.y;
}

bool Vec2Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 2, label);
}

Vec3Node::Vec3Node() : NumericNode<glm::vec3>("Vec3") {
    mValue = glm::vec3(0.0f);
}

std::vector<std::pair<std::string, std::string>> Vec3Node::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue.x << " " << mValue.y << " " << mValue.x;
    return {
        {"Value", stream.str()}
    };
}

void Vec3Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue.x >> mValue.y >> mValue.z;
}

bool Vec3Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 3, label);
}

Vec4Node::Vec4Node() : NumericNode<glm::vec4>("Vec4") {
    mValue = glm::vec4(0.0f);
}

std::vector<std::pair<std::string, std::string>> Vec4Node::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue.x << " " << mValue.y << " " << mValue.x << " " << mValue.w;
    return {
        {"Value", stream.str()}
    };
}

void Vec4Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue.x >> mValue.y >> mValue.z >> mValue.w;
}

bool Vec4Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 4, label);
}

Mat2Node::Mat2Node() : NumericNode<glm::mat2>("Matrix2") {
    mValue = glm::mat2(1.0f);
}

std::vector<std::pair<std::string, std::string>> Mat2Node::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue[0][0] << " " << mValue[0][1] << " "
           << mValue[1][0] << " " << mValue[1][1];
    return {
        {"Value", stream.str()}
    };
}

void Mat2Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue[0][0] >> mValue[0][1]
               >> mValue[1][0] >> mValue[1][1];
}

bool Mat2Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatNxN(&mValue[0][0], 2, label);
}

Mat3Node::Mat3Node() : NumericNode<glm::mat3>("Matrix3") {
    mValue = glm::mat3(1.0f);
}

std::vector<std::pair<std::string, std::string>> Mat3Node::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue[0][0] << " " << mValue[0][1] << " " << mValue[0][2] << " "
           << mValue[1][0] << " " << mValue[1][1] << " " << mValue[1][2] << " "
           << mValue[2][0] << " " << mValue[2][1] << " " << mValue[2][2];
    return {
        {"Value", stream.str()}
    };
}

void Mat3Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue[0][0] >> mValue[0][1] >> mValue[0][2]
               >> mValue[1][0] >> mValue[1][1] >> mValue[1][2]
               >> mValue[2][0] >> mValue[2][1] >> mValue[2][2];
}

bool Mat3Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatNxN(&mValue[0][0], 3, label);
}

Mat4Node::Mat4Node() : NumericNode<glm::mat4>("Matrix4") {
    mValue = glm::mat4(1.0f);
}

std::vector<std::pair<std::string, std::string>> Mat4Node::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue[0][0] << " " << mValue[0][1] << " " << mValue[0][2] << " " << mValue[0][3] << " "
           << mValue[1][0] << " " << mValue[1][1] << " " << mValue[1][2] << " " << mValue[1][3] << " "
           << mValue[2][0] << " " << mValue[2][1] << " " << mValue[2][2] << " " << mValue[2][3] << " "
           << mValue[3][0] << " " << mValue[3][1] << " " << mValue[3][2] << " " << mValue[3][3];
    return {
        {"Value", stream.str()}
    };
}

void Mat4Node::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue[0][0] >> mValue[0][1] >> mValue[0][2] >> mValue[0][3]
               >> mValue[1][0] >> mValue[1][1] >> mValue[1][2] >> mValue[1][3]
               >> mValue[2][0] >> mValue[2][1] >> mValue[2][2] >> mValue[2][3]
               >> mValue[3][0] >> mValue[3][1] >> mValue[3][2] >> mValue[3][3];
}

bool Mat4Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatNxN(&mValue[0][0], 4, label);
}
