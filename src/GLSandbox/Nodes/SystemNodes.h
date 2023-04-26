#pragma once
#include "../../NodeEditor/Node.h"

#include "../NodeClassifications.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/RenderConfig.h"

#include <glm/glm.hpp>

class ScreenWidthNode final : public Node {
public:
    ScreenWidthNode();
    ~ScreenWidthNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::ScreenWidth;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    Port<glm::vec2> mValueOut = Port<glm::vec2>(*this, IPort::Direction::Out, "ValueOut", "Value", [&]() { return RenderConfig::getDefaultViewportBounds(); });
};
