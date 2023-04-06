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

class PipelineGraph final : public Graph {
public:
    explicit PipelineGraph(IPipelineHandler& pipelineHandler);
    ~PipelineGraph() final = default;

    void initializeDefault() final;
protected:
    std::unique_ptr<Node> deserializeNodeType(unsigned int nodeType) final;

    void drawNodeCreation() final;
private:
    [[nodiscard]] std::unique_ptr<Node> generateNode(NodeType type) const;

    [[nodiscard]] static NodeGroup getNodeGroup(NodeType type);
    [[nodiscard]] static std::string getNodeName(NodeType type);
    [[nodiscard]] static std::string getGroupName(NodeGroup group);

    IPipelineHandler& mPipelineHandler;
};
