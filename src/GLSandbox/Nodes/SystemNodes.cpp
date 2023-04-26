#include "SystemNodes.h"

ScreenWidthNode::ScreenWidthNode() : Node("Screen Width") {
	addPort(mValueOut);
}

std::vector<std::pair<std::string, std::string>> ScreenWidthNode::generateSerializedData() const {
	return {};
}

void ScreenWidthNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
}

void ScreenWidthNode::drawContents() {
	ImGui::BeginDisabled();
	ImUtils::inputFloatN(&RenderConfig::getDefaultViewportBounds()[0], 2, generateNodeLabelID("Value"));
	ImGui::EndDisabled();
}
