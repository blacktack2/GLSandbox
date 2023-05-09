#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../NodeClassifications.h"

class LoopNode final : public Node {
public:
    LoopNode();
    ~LoopNode() override = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Loop;
    }

    [[nodiscard]] inline const Node* getLoopEntry() const {
        return mLoopOut.isLinked() ? mLoopOut.getLinkedValue<Node*>() : nullptr;
    }
    [[nodiscard]] inline const Node* getNextPass() const {
        return mExitOut.isLinked() ? mExitOut.getLinkedValue<Node*>() : nullptr;
    }

    [[nodiscard]] inline std::size_t getNumIterations() const {
        return mNumIterations;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    Port<Node*> mEntryIn  = Port<Node*>(*this, IPort::Direction::In, "Entry", "Entry", [&]() { return this; });
    Port<Node*> mExitOut = Port<Node*>(*this, IPort::Direction::Out, "Exit", "Exit", [&]() { return this; });

    Port<Node*> mLoopIn  = Port<Node*>(*this, IPort::Direction::In, "LoopEnd", "Loop-End", [&]() { return this; });
    Port<Node*> mLoopOut = Port<Node*>(*this, IPort::Direction::Out, "LoopBegin", "Loop-Begin", [&]() { return this; });

    std::size_t mNumIterations = 1;
};
