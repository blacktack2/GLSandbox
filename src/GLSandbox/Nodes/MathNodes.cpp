#include "MathNodes.h"

#include <imgui.h>

std::map<std::string, std::string> ArithmeticNode::generateSerializedData() const {
    return std::map<std::string, std::string>();
}

void ArithmeticNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void ArithmeticNode::drawContents() {
    drawOperationSelector();
}

void ArithmeticNode::drawOperationSelector() {
    const std::string COMBO_LABEL = std::string("##Combo_Node").append(std::to_string(getID()));
    if (ImGui::BeginCombo(COMBO_LABEL.c_str(), getOperationLabels()[(size_t)mCurrentOperation].c_str())) {
        for (size_t i = 0; i < getOperationLabels().size(); i++) {
            const bool isSelected = (Operation)i == mCurrentOperation;
            if (ImGui::Selectable(getOperationLabels()[i].c_str(), isSelected))
                mCurrentOperation = (Operation)i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
    }
}

std::any ArithmeticNode::calculateValue() {
    if (!mValueAIn.isLinked() || !mValueBIn.isLinked())
        return nullptr;

    const std::any a = mValueAIn.getLinkValue();
    const std::any b = mValueBIn.getLinkValue();
    const std::type_info& typeA = a.type();
    const std::type_info& typeB = b.type();
    if (typeA != typeB)
        return nullptr;

    if (typeA == typeid(int))
        return getOperations<int>().find(mCurrentOperation)->second(a, b);
    else if (typeA == typeid(float))
        return getOperations<float>().find(mCurrentOperation)->second(a, b);
    else
        assert(false);
}
