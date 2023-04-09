#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../NodeClassifications.h"

#include <glm/glm.hpp>

#include <functional>

template<typename T>
class NumericNode : public Node {
public:
    explicit NumericNode(const std::string& title) : Node(title) {
        addPort(mValueOut);
    }
    ~NumericNode() override = default;
protected:
    void drawContents() final {
        const std::string LABEL = std::string("##Input_Node_").append(std::to_string(getID()));
        if (drawInputArea(LABEL))
            mValueOut.valueUpdated();
    }
    virtual bool drawInputArea(const std::string& label) = 0;
protected:
    T mValue;
private:
    Port<T> mValueOut = Port<T>(*this, IPort::Direction::Out, "ValueOut", "Value", [&]() { return mValue; });
};

class IntegerNode final : public NumericNode<int> {
public:
    IntegerNode();
    ~IntegerNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Integer;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class FloatNode final : public NumericNode<float> {
public:
    FloatNode();
    ~FloatNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Float;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class Vec2Node final : public NumericNode<glm::vec2> {
public:
    Vec2Node();
    ~Vec2Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Vec2;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class Vec3Node final : public NumericNode<glm::vec3> {
public:
    Vec3Node();
    ~Vec3Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Vec3;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class Vec4Node final : public NumericNode<glm::vec4> {
public:
    Vec4Node();
    ~Vec4Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Vec4;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class Mat2Node final : public NumericNode<glm::mat2> {
public:
    Mat2Node();
    ~Mat2Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Mat2;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class Mat3Node final : public NumericNode<glm::mat3> {
public:
    Mat3Node();
    ~Mat3Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Mat3;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class Mat4Node final : public NumericNode<glm::mat4> {
public:
    Mat4Node();
    ~Mat4Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Mat4;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};
