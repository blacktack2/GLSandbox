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
    typedef std::function<std::unique_ptr<Node>()> factory_callback;

    IPipelineHandler& mPipelineHandler;

    std::unordered_map<NodeType, factory_callback> mNodeFactories;
    std::map<NodeGroup, std::vector<NodeType>> mNodeGroups;
};
