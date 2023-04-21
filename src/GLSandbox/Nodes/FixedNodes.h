#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

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
    void pipelineUpdateEvent();
    void pipelineResetEvent();

    bool validatePipeline() const;
    void updatePipeline();

    IPipelineHandler& mPipelineHandler;

    Port<void*> mExecutionOut = Port<void*>(*this, IPort::Direction::Out, "Out", "Out", [&]() { return (void*)nullptr; });

    mutable std::string mMessage;
    mutable MessageType mMessageType = MessageType::Info;
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
    Port<void*> mExecutionIn = Port<void*>(*this, IPort::Direction::In, "In", "In");
};
