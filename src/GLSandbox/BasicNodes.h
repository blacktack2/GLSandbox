#pragma once
#include "../NodeEditor/Node.h"

class IntegerNode : public Node {
public:
    IntegerNode() : Node("Integer"), mIntOut("Value", mValue) {
        addPort(mIntOut);
    }
protected:
    void drawContents() override;
private:
    int mValue = 0;

    OutPort mIntOut;
};

class FloatNode : public Node {
public:
    FloatNode() : Node("Float"), mFloatOut("Value", mValue) {
        addPort(mFloatOut);
    }
protected:
    void drawContents() override;
private:
    float mValue = 0.0f;

    OutPort mFloatOut;
};
