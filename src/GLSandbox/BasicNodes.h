#pragma once
#include "../NodeEditor/Node.h"

#include <any>
#include <functional>

class IntegerNode : public Node {
public:
    IntegerNode() : Node("Integer") {
        addPort(mIntOut);
    }
protected:
    void drawContents() override;
private:
    int mValue = 0;
    std::any mValueWrapper = std::ref(mValue);
    OutPort mIntOut = OutPort(*this, "Value", mValueWrapper);
};

class FloatNode : public Node {
public:
    FloatNode() : Node("Float") {
        addPort(mFloatOut);
    }
protected:
    void drawContents() override;
private:
    float mValue = 0.0f;
    std::any mValueWrapper = std::ref(mValue);
    OutPort mFloatOut = OutPort(*this, "Value", mValueWrapper);
};
