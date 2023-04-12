#include "MathNodes.h"

#include <imgui.h>

std::vector<std::pair<std::string, std::string>> ArithmeticNode::generateSerializedData() const {
    return {};
}

void ArithmeticNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void ArithmeticNode::drawContents() {
    drawOperationSelector();
}

void ArithmeticNode::drawOperationSelector() {
    const std::string operationComboLabel = generateNodeLabel("", "Combo");
    ImGui::SetNextItemWidth(getComboWidth());
    if (ImGui::BeginCombo(operationComboLabel.c_str(), getOperationLabels()[(size_t)mCurrentOperation].c_str())) {
        for (size_t i = 0; i < getOperationLabels().size(); i++) {
            const bool isSelected = (Operation)i == mCurrentOperation;
            ImGui::SetNextItemWidth(getComboItemWidth());
            if (ImGui::Selectable(getOperationLabels()[i].c_str(), isSelected))
                mCurrentOperation = (Operation)i;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
    }
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
