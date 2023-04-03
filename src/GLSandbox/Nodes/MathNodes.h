#pragma once
#include "../../NodeEditor/Node.h"

#include "../NodeClassifications.h"
#include "BasicNodes.h"

#include <functional>
#include <string>
#include <unordered_map>
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
    typedef std::function<std::any(const std::any&, const std::any&)> operation_callback;

    enum class Operation {
        Add = 0ull,
        Subtract,
        Multiply,
        Divide,
    };

    void drawOperationSelector();

    std::any calculateValue();

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

    template<typename T>
    static inline const std::unordered_map<Operation, operation_callback>& getOperations() {
        static const std::unordered_map<Operation, operation_callback> OPERATIONS = {
            {Operation::Add,      [](const std::any& a, const std::any& b) { return std::any(std::any_cast<T>(a) + std::any_cast<T>(b)); }},
            {Operation::Subtract, [](const std::any& a, const std::any& b) { return std::any(std::any_cast<T>(a) - std::any_cast<T>(b)); }},
            {Operation::Multiply, [](const std::any& a, const std::any& b) { return std::any(std::any_cast<T>(a) * std::any_cast<T>(b)); }},
            {Operation::Divide,   [](const std::any& a, const std::any& b) { return std::any(std::any_cast<T>(a) / std::any_cast<T>(b)); }},
        };
        return OPERATIONS;
    };

    InPort mValueAIn = InPort(*this, "A", {&typeid(float), &typeid(int)});
    InPort mValueBIn = InPort(*this, "B", {&typeid(float), &typeid(int)});

    OutPort mValueOut = OutPort(*this, "Out", [&]() { return calculateValue(); });

    Operation mCurrentOperation = Operation::Add;
};