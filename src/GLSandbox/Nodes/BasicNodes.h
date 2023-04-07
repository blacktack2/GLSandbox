#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../NodeClassifications.h"

#include <glm/glm.hpp>

#include <functional>

class IntegerNode final : public Node {
public:
    IntegerNode();
    ~IntegerNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Integer;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    int mValue = 0;
    Port<int> mValueOut = Port<int>(*this, IPort::Direction::Out, "IntOut", "Value", [&]() { return mValue; });
};

class FloatNode final : public Node {
public:
    FloatNode();
    ~FloatNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Float;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    float mValue = 0.0f;
    Port<float> mValueOut = Port<float>(*this, IPort::Direction::Out, "FloatOut", "Value", [&]() { return mValue; });
};

class Vec2Node final : public Node {
public:
    Vec2Node();
    ~Vec2Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Vec2;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    glm::vec2 mValue = glm::vec2(0.0f);
    Port<glm::vec2> mValueOut = Port<glm::vec2>(*this, IPort::Direction::Out, "VecOut", "Value", [&]() { return mValue; });
};

class Vec3Node final : public Node {
public:
    Vec3Node();
    ~Vec3Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Vec3;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    glm::vec3 mValue = glm::vec3(0.0f);
    Port<glm::vec3> mValueOut = Port<glm::vec3>(*this, IPort::Direction::Out, "VecOut", "Value", [&]() { return mValue; });
};

class Vec4Node final : public Node {
public:
    Vec4Node();
    ~Vec4Node() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Vec4;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    glm::vec4 mValue = glm::vec4(0.0f);
    Port<glm::vec4> mValueOut = Port<glm::vec4>(*this, IPort::Direction::Out, "VecOut", "Value", [&]() { return mValue; });
};
