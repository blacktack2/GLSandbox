#pragma once
#include "../NodeEditor/Graph.h"

#include "../Rendering/IPipelineHandler.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class PipelineGraph : public Graph {
public:
    explicit PipelineGraph(IPipelineHandler& pipelineHandler);
protected:
    void drawNodeCreation() final;
private:
    struct NodeFactory {
        typedef std::function<std::unique_ptr<Node>()> factory_callback;

        NodeFactory(const std::string& label, factory_callback callback) : mCallback(std::move(callback)) {
            static int cLabelIDCounter = 0;
            mLabel = std::string(label).append("##NodeCreator_").append(std::to_string(cLabelIDCounter++));
        }

        std::string mLabel;
        factory_callback mCallback;
    };

    void drawNodeSelectors(const std::vector<NodeFactory>& factories);

    IPipelineHandler& mPipelineHandler;
};
