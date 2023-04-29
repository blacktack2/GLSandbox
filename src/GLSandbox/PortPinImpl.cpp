#include "../NodeEditor/Ports.cpp"

void parsePinTypeData(size_t pinHash, float& pinSize, PinShape& shape, ImU32& colour, std::string& tooltip) {
    if (pinHash == typeHash<void*>()) {
        pinSize = 14.0f;
        shape = PinShape::Arrow;
        colour = ImColor(1.0f, 1.0f, 1.0f);
        tooltip = "Execution";
    } else if (pinHash == typeHash<int>()) {
        pinSize = 10.0f;
        shape = PinShape::Circle;
        colour = ImColor(1.0f, 1.0f, 0.0f);
        tooltip = "Integer";
    } else if (pinHash == typeHash<glm::ivec2>()) {
        pinSize = 10.0f;
        shape = PinShape::Triangle;
        colour = ImColor(0.8f, 0.8f, 0.0f);
        tooltip = "IVec2";
    } else if (pinHash == typeHash<glm::ivec3>()) {
        pinSize = 10.0f;
        shape = PinShape::Triangle;
        colour = ImColor(0.6f, 0.6f, 0.0f);
        tooltip = "IVec3";
    } else if (pinHash == typeHash<glm::ivec4>()) {
        pinSize = 10.0f;
        shape = PinShape::Triangle;
        colour = ImColor(0.4f, 0.4f, 0.0f);
        tooltip = "IVec4";
    } else if (pinHash == typeHash<float>()) {
        pinSize = 10.0f;
        shape = PinShape::Circle;
        colour = ImColor(0.0f, 1.0f, 1.0f);
        tooltip = "Float";
    } else if (pinHash == typeHash<glm::vec2>()) {
        pinSize = 10.0f;
        shape = PinShape::Triangle;
        colour = ImColor(0.0f, 1.0f, 0.7f);
        tooltip = "Vec2";
    } else if (pinHash == typeHash<glm::vec3>()) {
        pinSize = 10.0f;
        shape = PinShape::Triangle;
        colour = ImColor(0.0f, 1.0f, 0.4f);
        tooltip = "Vec3";
    } else if (pinHash == typeHash<glm::vec4>()) {
        pinSize = 10.0f;
        shape = PinShape::Triangle;
        colour = ImColor(0.0f, 1.0f, 0.0f);
        tooltip = "Vec4";
    } else if (pinHash == typeHash<glm::mat2>()) {
        pinSize = 8.0f;
        shape = PinShape::Square;
        colour = ImColor(0.0f, 1.0f, 0.7f);
        tooltip = "Matrix2x2";
    } else if (pinHash == typeHash<glm::mat3>()) {
        pinSize = 8.0f;
        shape = PinShape::Square;
        colour = ImColor(0.0f, 1.0f, 0.4f);
        tooltip = "Matrix3x3";
    } else if (pinHash == typeHash<glm::mat4>()) {
        pinSize = 8.0f;
        shape = PinShape::Square;
        colour = ImColor(0.0f, 1.0f, 0.0f);
        tooltip = "Matrix4x4";
    } else if (pinHash == typeHash<Framebuffer*>()) {
        pinSize = 10.0f;
        shape = PinShape::FlatSquare;
        colour = ImColor(1.0f, 1.0f, 1.0f);
        tooltip = "Framebuffer";
    } else if (pinHash == typeHash<Mesh*>()) {
        pinSize = 10.0f;
        shape = PinShape::FlatSquare;
        colour = ImColor(0.0f, 0.8f, 1.0f);
        tooltip = "Mesh";
    } else if (pinHash == typeHash<Shader*>()) {
        pinSize = 10.0f;
        shape = PinShape::FlatSquare;
        colour = ImColor(1.0f, 0.4f, 0.0f);
        tooltip = "Shader";
    } else if (pinHash == typeHash<Texture*>()) {
        pinSize = 10.0f;
        shape = PinShape::FlatSquare;
        colour = ImColor(0.0f, 1.0f, 0.0f);
        tooltip = "Texture";
    } else if (pinHash == typeHash<float, int>()) {
        pinSize = 10.0f;
        shape = PinShape::Circle;
        colour = ImColor(1.0f, 1.0f, 0.0f);
        tooltip = "Number";
    } else {
        pinSize = 8.0f;
        shape = PinShape::Circle;
        colour = ImColor(1.0f, 1.0f, 1.0f);
        tooltip = "";
    }
}
