#pragma once
#include "../../NodeEditor/Node.h"

#include "../NodeClassifications.h"
#include "BasicNodes.h"

#include <functional>
#include <string>
#include <vector>

class ArithmeticNode final : public Node {
public:
    ArithmeticNode() : Node("Integer") {
        addPort(mValueAIn);
        addPort(mValueBIn);
        addPort(mValueOut);
    }
    ~ArithmeticNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Arithmetic;
    }
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() final;
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