#include "MathNodes.h"

#include <imgui.h>

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

void ArithmeticNode::serializeContents(std::ofstream& streamOut) const {

}

void ArithmeticNode::deserializeContents(std::ifstream& streamIn) {

}
