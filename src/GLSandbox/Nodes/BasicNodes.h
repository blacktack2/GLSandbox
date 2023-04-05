#pragma once
#include "../../NodeEditor/Node.h"

#include "../NodeClassifications.h"

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
    OutPort mIntOut = OutPort(*this, "IntOut", "Value", [&]() { return mValue; });
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
    OutPort mFloatOut = OutPort(*this, "FloatOut", "Value", [&]() { return mValue; });
};
