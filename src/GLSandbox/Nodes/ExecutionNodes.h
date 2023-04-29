#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/IPipelineHandler.h"
#include "../NodeClassifications.h"

class EntryNode final : public Node {
public:
    explicit EntryNode(IPipelineHandler& pipelineHandler);
    ~EntryNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
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

class InputNode final : public Node {
public:
    InputNode();
    ~InputNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Input;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
};

class OutputNode final : public Node {
public:
    OutputNode();
    ~OutputNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Output;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
};
