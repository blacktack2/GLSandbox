#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../NodeClassifications.h"
#include "BasicNodes.h"

#include <glm/glm.hpp>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class ArithmeticNode final : public Node {
public:
    typedef std::variant<
        int,   glm::ivec2, glm::ivec3, glm::ivec4,
        float, glm::vec2,  glm::vec3,  glm::vec4
    > numeric_variant_t;
    typedef Port<
        int, glm::ivec2, glm::ivec3, glm::ivec4,
        float, glm::vec2, glm::vec3, glm::vec4
    > numeric_port_t;

    ArithmeticNode();
    ~ArithmeticNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Arithmetic;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    enum class Operation : size_t {
        Add = 0,
        Subtract,
        Multiply,
        Divide,

        Max,
    };

    numeric_variant_t calculateValue();

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

    numeric_port_t mValueAIn = numeric_port_t(*this, IPort::Direction::In, "ValueAIn", "A");
    std::unique_ptr<IPort> mValueBIn;

    std::unique_ptr<IPort> mValueOut;

    Operation mCurrentOperation = Operation::Add;
};