#pragma once
#include "../../NodeEditor/Node.h"

#include "../../Rendering/IPipelineHandler.h"
#include "../NodeClassifications.h"

class EntryNode final : public Node {
public:
    explicit EntryNode(IPipelineHandler& pipelineHandler);
    ~EntryNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Entry;
    }
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

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

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Exit;
    }
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "In");
};
