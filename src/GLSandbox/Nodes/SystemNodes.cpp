#include "SystemNodes.h"

ScreenWidthNode::ScreenWidthNode() : Node("Screen Width") {
	addPort(mValueOut);

	RenderConfig::addResizeCallable(*this);
}

ScreenWidthNode::~ScreenWidthNode() {
	RenderConfig::removeResizeCallable(*this);
}

void ScreenWidthNode::onResizeEvent() {
	mValueOut.valueUpdated();
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
