#pragma once
#include "../NodeEditor/Node.h"
#include "BasicNodes.h"

#include <functional>
#include <string>
#include <vector>

class ArithmeticNode : public Node {
public:
    ArithmeticNode() : Node("Integer") {
        addPort(mValueAIn);
        addPort(mValueBIn);
        addPort(mValueOut);
    }
protected:
    void drawContents() override;
private:
    enum class Operation {
        Add = 0ull,
        Subtract,
        Multiply,
        Divide,
    };

    void drawOperationSelector();

    static inline const std::vector<std::string>& getOperationLabels() {
        // Matches Operation enum
        static const std::vector<std::string> OPERATIONS = {
            "Add",
            "Subtract",
            "Multiply",
            "Divide",
        };
        return OPERATIONS;
    }

    InPort mValueAIn = InPort(*this, "A", {&typeid(IntegerNode), &typeid(FloatNode)});
    InPort mValueBIn = InPort(*this, "B", {&typeid(IntegerNode), &typeid(FloatNode)});

    OutPort mValueOut = OutPort(*this, "Out");

    Operation mCurrentOperation = Operation::Add;
};