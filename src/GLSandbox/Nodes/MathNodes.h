#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../NodeClassifications.h"
#include "BasicNodes.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class ArithmeticNode final : public Node {
public:
    ArithmeticNode() : Node("Arithmetic") {
        addPort(mValueAIn);
        addPort(mValueBIn);
        addPort(mValueOut);
    }
    ~ArithmeticNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Arithmetic;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    enum class Operation {
        Add = 0,
        Subtract,
        Multiply,
        Divide,

        Max,
    };

    std::variant<float, int> calculateValue();

    static inline std::string getOperationLabel(Operation operation) {
        switch (operation) {
            default: return "Undefined";
            case Operation::Add: return "Add";
            case Operation::Subtract: return "Subtract";
            case Operation::Multiply: return "Multiply";
            case Operation::Divide: return "Divide";
        }
    }

    template<typename T1, typename T2>
    static inline T1 operate(Operation operation, T1 a, T2 b) {
        switch (operation) {
            default:
            case Operation::Add      : return a + b;
            case Operation::Subtract : return a - b;
            case Operation::Multiply : return a * b;
            case Operation::Divide   : return a / b;
        }
    };

    Port<float, int> mValueAIn = Port<float, int>(*this, IPort::Direction::In, "ValueAIn", "A");
    Port<float, int> mValueBIn = Port<float, int>(*this, IPort::Direction::In, "ValueBIn", "B");

    Port<float, int> mValueOut = Port<float, int>(*this, IPort::Direction::Out, "ValueOut", "Out", [&]() { return calculateValue(); });

    Operation mCurrentOperation = Operation::Add;
};