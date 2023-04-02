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
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() final;
private:
    int mValue = 0;
    OutPort mIntOut = OutPort(*this, "Value");
};

class FloatNode final : public Node {
public:
    FloatNode();
    ~FloatNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Float;
    }
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() final;
private:
    float mValue = 0.0f;
    OutPort mFloatOut = OutPort(*this, "Value");
};
