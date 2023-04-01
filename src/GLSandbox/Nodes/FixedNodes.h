#pragma once
#include "../../NodeEditor/Node.h"

#include "../../Rendering/IPipelineHandler.h"

class EntryNode final : public Node {
public:
    explicit EntryNode(IPipelineHandler& pipelineHandler);
    ~EntryNode() final = default;
protected:
    void drawContents() final;
private:
    bool validatePipeline();
    void updatePipeline();

    IPipelineHandler& mPipelineHandler;

    OutPort mExecutionOutPort = OutPort(*this, "Out");
};

class ExitNode final : public Node {
public:
    ExitNode();
    ~ExitNode() final = default;
protected:
    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "In");
};
