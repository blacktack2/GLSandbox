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
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    bool validatePipeline();
    void updatePipeline();

    IPipelineHandler& mPipelineHandler;

    OutPort mExecutionOutPort = OutPort(*this, "ExecutionOut", "Out", [&]() { return (void*)nullptr; });
};

class ExitNode final : public Node {
public:
    ExitNode();
    ~ExitNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Exit;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "ExecutionIn", "In");
};
