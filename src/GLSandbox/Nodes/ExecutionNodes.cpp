#include "ExecutionNodes.h"

#include "RenderPassNode.h"

#include "../Assets.h"
#include "../PipelineGraph.h"

#include "../../Utils/SerializationUtils.h"

EntryNode::EntryNode(IPipelineHandler& pipelineHandler) : Node("Entry"), mPipelineHandler(pipelineHandler) {
    addPort(mExecutionOut);
}

std::vector<std::pair<std::string, std::string>> EntryNode::generateSerializedData() const {
    return {};
}

void EntryNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void EntryNode::drawContents() {
    if (!isLocked())
        if (ImUtils::button("Update", generateNodeLabelID("Update")))
            pipelineUpdateEvent();
    if (isLocked())
        if (ImUtils::button("Reset", generateNodeLabelID("Reset")))
            pipelineResetEvent();

    if (!mMessage.empty())
        drawMessage(mMessage, getMessageColour(mMessageType));
}

void EntryNode::pipelineUpdateEvent() {
    if (validatePipeline())
        updatePipeline();
    else
        mPipelineHandler.resetPipeline();
}

void EntryNode::pipelineResetEvent() {
    unlock();
    mPipelineHandler.resetPipeline();
}

bool EntryNode::validatePipeline() const {
    if (!mExecutionOut.isLinked()) {
        mMessage = "Not linked to a render pass";
        mMessageType = MessageType::Error;
        return false;
    }
    bool isValid = true;
    const Node* node = mExecutionOut.getLinkedValue<Node*>();
    while (node) {
        const Node* next = nullptr;
        if (const RenderPassNode* renderPassNode = dynamic_cast<const RenderPassNode*>(node)) {
            isValid &= renderPassNode->validate();
            next = renderPassNode->getNextPass();
        } else if (const InputNode* inputNode = dynamic_cast<const InputNode*>(node)) {
            if (isValid &= inputNode->validate())
                next = inputNode->getOutputValue<Node*>();
        } else if (const OutputNode* outputNode = dynamic_cast<const OutputNode*>(node)) {
            isValid &= outputNode->validate();
        } else {
            mMessage = std::string("Unsupported connection [").append(node->getName()).append("]");
            return false;
        }

        if (next == node) {
            mMessage = "Infinite loop detected";
            return false;
        }
        node = next;
    }
    if (isValid) {
        mMessage = "Uploaded";
        mMessageType = MessageType::Confirmation;
        return true;
    } else {
        mMessage = "Invalid";
        mMessageType = MessageType::Error;
        return false;
    }
}

void EntryNode::updatePipeline() {
    mPipelineHandler.clearPipeline();

    const Node* node = mExecutionOut.getLinkedValue<Node*>();
    while (node) {
        if (const RenderPassNode* renderPassNode = dynamic_cast<const RenderPassNode*>(node)) {
            mPipelineHandler.appendPipeline(renderPassNode->generateCallback());
            node = renderPassNode->getNextPass();
        } else if (const InputNode* inputNode = dynamic_cast<const InputNode*>(node)) {
            node = inputNode->getOutputValue<Node*>();
        } else if (dynamic_cast<const OutputNode*>(node)) {
            node = nullptr;
        } else {
            assert(false);
        }
    }

    mPipelineHandler.appendPipeline([]() {
        RenderConfig::setViewport();
        RenderConfig::setClearColour();
        RenderConfig::setBlend();
        RenderConfig::setColourMask();
        RenderConfig::setCullFace();
        RenderConfig::setDepthTest();
        RenderConfig::setDepthMask();
    });

    lock();
}

InputNode::InputNode() : Node("Input") {
    addPort(mDefaultIn);

    mValueOut = nullptr;

    mDefaultIn.addOnLinkEvent([this]() {
        std::visit([this](const auto& arg) {
            using port_type = std::decay_t<decltype(arg)>;
            mValueOut = std::make_unique<Port<port_type>>(*this, IPort::Direction::Out, "Out", "Out", [this]() {
                return std::visit(VisitOverload{
                    [](const auto& arg2) { return port_type{}; },
                    [](const port_type& arg2) { return arg2; },
                }, mExternalInput ? *mExternalInput : mDefaultIn.getLinkedValue());
            }, false, true);
            addPort(*mValueOut);
        }, mDefaultIn.getLinkedValue());
    });
    mDefaultIn.addOnUnlinkEvent([this]() {
        removePort(*mValueOut);
        mValueOut = nullptr;
    });
}

bool InputNode::validate() const {
    return isValid() == ValidationState::Valid;
}

void InputNode::drawInput() {
    ImGui::Text("%s -> ", getName().c_str());

    ImGui::SameLine();

    switch (isValid()) {
        case ValidationState::Valid:
            ImGui::BeginGroup();

            std::visit(VisitOverload{
                [&](int value) {
                    if (ImUtils::inputIntN(&value, 1, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::ivec2 value) {
                    if (ImUtils::inputIntN(&value[0], 2, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::ivec3 value) {
                    if (ImUtils::inputIntN(&value[0], 3, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::ivec4 value) {
                    if (ImUtils::inputIntN(&value[0], 4, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](float value) {
                    if (ImUtils::inputFloatN(&value, 1, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::vec2 value) {
                    if (ImUtils::inputFloatN(&value[0], 2, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::vec3 value) {
                    if (ImUtils::inputFloatN(&value[0], 3, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::vec4 value) {
                    if (ImUtils::inputFloatN(&value[0], 4, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::mat2 value) {
                    if (ImUtils::inputFloatNxN(&value[0][0], 2, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::mat3 value) {
                    if (ImUtils::inputFloatNxN(&value[0][0], 3, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](glm::mat4 value) {
                    if (ImUtils::inputFloatNxN(&value[0][0], 4, generateNodeLabelID("InputValue")))
                        setValue(value);
                },
                [&](const auto& value) { drawMessage("Undefined", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); },
            }, mExternalInput ? *mExternalInput : mDefaultIn.getLinkedValue());

            ImGui::EndGroup();

            ImGui::SameLine();

            if (ImUtils::button("Reset", generateNodeLabelID("InputReset")))
                mExternalInput = nullptr;

            break;
        default:
        case ValidationState::Unlinked:
            drawMessage("Must be linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            break;
    }
}

std::vector<std::pair<std::string, std::string>> InputNode::generateSerializedData() const {
    return {};
}

void InputNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void InputNode::drawContents() {
    if (!mDefaultIn.isLinked())
        drawMessage("Must have a default value linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    else if (!mValueOut->isLinked())
        drawMessage("Output should be linked to something", ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
}

OutputNode::OutputNode() : Node("Output") {
    addPort(mValueIn);
}

bool OutputNode::validate() const {
    return isValid() == ValidationState::Valid;
}

void OutputNode::drawOutput() {
    ImGui::Text("%s -> ", getName().c_str());

    ImGui::SameLine();

    switch (isValid()) {
        case ValidationState::Valid:
            ImGui::BeginGroup();

            std::visit(VisitOverload{
                [&](const int&        value) { ImGui::Text("%d", value); },
                [&](const glm::ivec2& value) { ImGui::Text("%d, %d", value.x, value.y); },
                [&](const glm::ivec3& value) { ImGui::Text("%d, %d, %d", value.x, value.y, value.z); },
                [&](const glm::ivec4& value) { ImGui::Text("%d, %d, %d, %d", value.x, value.y, value.z, value.w); },
                [&](const float&     value) { ImGui::Text("%.2f", value); },
                [&](const glm::vec2& value) { ImGui::Text("%.2f, %.2f", value.x, value.y); },
                [&](const glm::vec3& value) { ImGui::Text("%.2f, %.2f, %.2f", value.x, value.y, value.z); },
                [&](const glm::vec4& value) { ImGui::Text("%.2f, %.2f, %.2f, %.2f", value.x, value.y, value.z, value.w); },
                [&](const glm::mat2& value) {
                    ImGui::Text("%.2f, %.2f", value[0][0], value[0][1]);
                    ImGui::Text("%.2f, %.2f", value[1][0], value[1][1]);
                },
                [&](const glm::mat3& value) {
                    ImGui::Text("%.2f, %.2f, %.2f", value[0][0], value[0][1], value[0][2]);
                    ImGui::Text("%.2f, %.2f, %.2f", value[1][0], value[1][1], value[1][2]);
                    ImGui::Text("%.2f, %.2f, %.2f", value[2][0], value[2][1], value[2][2]);
                },
                [&](const glm::mat4& value) {
                    ImGui::Text("%.2f, %.2f, %.2f, %.2f", value[0][0], value[0][1], value[0][2], value[0][3]);
                    ImGui::Text("%.2f, %.2f, %.2f, %.2f", value[1][0], value[1][1], value[1][2], value[1][3]);
                    ImGui::Text("%.2f, %.2f, %.2f, %.2f", value[2][0], value[2][1], value[2][2], value[2][3]);
                    ImGui::Text("%.2f, %.2f, %.2f, %.2f", value[3][0], value[3][1], value[3][2], value[3][3]);
                },
                [&](const auto& value) { drawMessage("Undefined", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); },
            }, mValueIn.getLinkedValue());

            ImGui::EndGroup();
            break;
        default:
        case ValidationState::Unlinked:
            drawMessage("Must be linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            break;
    }
}

std::vector<std::pair<std::string, std::string>> OutputNode::generateSerializedData() const {
    return {};
}

void OutputNode::deserializeData(const std::string& dataID, std::ifstream& stream) {

}

void OutputNode::drawContents() {
    if (!mValueIn.isLinked())
        drawMessage("Must have a value linked", ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
}

SubGraphNode::SubGraphNode(Graph& parent, IPipelineHandler& pipelineHandler) : Node("Sub-Graph"),
mParent(parent), mPipelineHandler(pipelineHandler) {
    mGraph = nullptr;
}

std::vector<std::pair<std::string, std::string>> SubGraphNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data;
    data.emplace_back("File", mGraphFilepath.generic_string());
    return data;
}

void SubGraphNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "File")
        mGraphFilepath = SerializationUtils::readLine(stream);
}

void SubGraphNode::onDeserialize() {
    loadFromFile();
}

void SubGraphNode::drawContents() {
    if (ImUtils::fileChooseDialog(mGraphFilepath, getGraphAssetDirectory(), generateNodeLabelID("File"),
                                  getValidGraphFileExtensions()))
        loadFromFile();
}

void SubGraphNode::loadFromFile() {
    mGraph = nullptr;
    mInputs.clear();
    mOutputs.clear();

    std::ifstream stream(mGraphFilepath);
    if (!stream)
        return;

    std::streampos begin = stream.tellg();

    long id;
    stream >> id;
    if (mParent.isDependency(id))
        return;

    stream.seekg(begin);
    mGraph = std::make_unique<PipelineGraph>(mPipelineHandler);
    mGraph->deserialize(stream);

    updatePorts();
}

void SubGraphNode::updatePorts() {
    const auto& inputs  = ((PipelineGraph&)*mGraph).getInputNodes();
    const auto& outputs = ((PipelineGraph&)*mGraph).getOutputNodes();

    for (InputNode& node : inputs) {
        std::unique_ptr<IPort> port = std::visit([this, &node](const auto& arg)->std::unique_ptr<IPort> {
            using value_t = std::decay_t<decltype(arg)>;
            std::string uniqueName = std::string("Input-").append(std::to_string(mInputs.size()));
            auto port = std::make_unique<Port<value_t>>(*this, IPort::Direction::In, uniqueName, node.getName(),
                                                        nullptr, false, true);
            Port<value_t>* rawPort = port.get();
            port->addOnUpdateEvent([&node, rawPort]() {
                node.setValue(rawPort->template getLinkedValue<value_t>());
            });
            return port;
        }, node.getDefaultValue());

        addPort(*port);
        mInputs.push_back(std::move(port));
    }
    for (OutputNode& node : outputs) {
        std::unique_ptr<IPort> port = std::visit([this, &node](const auto& arg)->std::unique_ptr<IPort> {
            using value_t = std::decay_t<decltype(arg)>;
            std::string uniqueName = std::string("Output-").append(std::to_string(mOutputs.size()));
            return std::make_unique<Port<value_t>>(*this, IPort::Direction::Out, uniqueName, node.getName(),
                [&node]() {
                    return node.getValue<value_t>();
                }, false, true);
        }, node.getValue());

        addPort(*port);
        mOutputs.push_back(std::move(port));
    }
}
