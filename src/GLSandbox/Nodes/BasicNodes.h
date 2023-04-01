#pragma once
#include "../../NodeEditor/Node.h"

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
    OutPort mIntOut = OutPort(*this, "Value");
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
    OutPort mFloatOut = OutPort(*this, "Value");
};
