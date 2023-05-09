#include "FlowControlNodes.h"

#include "../../Utils/SerializationUtils.h"

LoopNode::LoopNode() : Node("Loop") {
    addPort(mEntryIn);
    addPort(mExitOut);
    addPort(mLoopIn);
    addPort(mLoopOut);
}

std::vector<std::pair<std::string, std::string>> LoopNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Iterations", SerializationUtils::serializeData(mNumIterations));
    return data;
}

void LoopNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Iterations")
        SerializationUtils::deserializeData(stream, mNumIterations);
}

void LoopNode::drawContents() {
    ImGui::BeginDisabled(isLocked());

    ImUtils::inputInt((int*)&mNumIterations, generateNodeLabelID("Iterations"), 0, INT_MAX);

    ImGui::EndDisabled();
}
