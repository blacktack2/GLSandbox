#pragma once
#include "../NodeEditor/Graph.h"

#include "../Rendering/IPipelineHandler.h"

#include "NodeClassifications.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class InputNode;
class OutputNode;

class PipelineGraph final : public Graph {
public:
    explicit PipelineGraph(IPipelineHandler& pipelineHandler);
    ~PipelineGraph() final = default;

    void initializeDefault() final;
protected:
    std::unique_ptr<Node> deserializeNodeType(const std::string& nodeType) final;
    [[nodiscard]] std::string getNodeSerialName(const Node& node) const final;

    void drawNodeCreation() final;
    void drawInputPanelContents() final;
    void drawOutputPanelContents() final;

    void onClear() final;
    void onNodeAdd(Node& node) final;
    void onNodeDelete(Node& node) final;
private:
    [[nodiscard]] std::unique_ptr<Node> generateNode(NodeType type) const;

    [[nodiscard]] static std::string getGroupName(NodeGroup group);

    std::vector<std::reference_wrapper<InputNode>> mInputNodes{};
    std::vector<std::reference_wrapper<OutputNode>> mOutputNodes{};

    IPipelineHandler& mPipelineHandler;
};
