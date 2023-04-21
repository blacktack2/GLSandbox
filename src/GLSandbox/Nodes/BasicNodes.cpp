#include "BasicNodes.h"

#include <sstream>

IntegerNode::IntegerNode() : NumericNode<int>("Integer") {
    mValue = 0;
}

bool IntegerNode::drawInputArea(const std::string& label) {
    return ImUtils::inputIntN(&mValue, 1, label);
}

FloatNode::FloatNode() : NumericNode<float>("Float") {
    mValue = 0.0f;
}

bool FloatNode::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue, 1, label);
}

Vec2Node::Vec2Node() : NumericNode<glm::vec2>("Vec2") {
    mValue = glm::vec2(0.0f);
}

bool Vec2Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 2, label);
}

Vec3Node::Vec3Node() : NumericNode<glm::vec3>("Vec3") {
    mValue = glm::vec3(0.0f);
}

bool Vec3Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 3, label);
}

Vec4Node::Vec4Node() : NumericNode<glm::vec4>("Vec4") {
    mValue = glm::vec4(0.0f);
}

bool Vec4Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 4, label);
}

Mat2Node::Mat2Node() : NumericNode<glm::mat2>("Matrix2") {
    mValue = glm::mat2(1.0f);
}

bool Mat2Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatNxN(&mValue[0][0], 2, label);
}

Mat3Node::Mat3Node() : NumericNode<glm::mat3>("Matrix3") {
    mValue = glm::mat3(1.0f);
}

bool Mat3Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatNxN(&mValue[0][0], 3, label);
}

Mat4Node::Mat4Node() : NumericNode<glm::mat4>("Matrix4") {
    mValue = glm::mat4(1.0f);
}

bool Mat4Node::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatNxN(&mValue[0][0], 4, label);
}
