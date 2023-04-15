#include "MathNodes.h"

std::vector<std::pair<std::string, std::string>> ArithmeticNode::generateSerializedData() const {
    return {};
}

void ArithmeticNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void ArithmeticNode::drawContents() {
    ImUtils::cycleButton(generateNodeLabelID("Combo"), (size_t&)mCurrentOperation, (size_t)Operation::Max,
                         [](size_t index) { return getOperationLabel((Operation)index); });
}

std::variant<float, int> ArithmeticNode::calculateValue() {
    if (!mValueAIn.isLinked() || !mValueBIn.isLinked())
        return 0;

    std::variant<float, int> valueA = mValueAIn.getConnectedValue();
    std::variant<float, int> valueB = mValueBIn.getConnectedValue();

    return std::visit(
        [this](const auto& a, const auto& b)->std::variant<float, int> {
            return operate(mCurrentOperation, a, b);
        }, valueA, valueB
    );
}
