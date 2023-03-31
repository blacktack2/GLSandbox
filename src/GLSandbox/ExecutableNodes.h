#pragma once
#include "../NodeEditor/Node.h"

class EntryNode final : public Node {
public:
    EntryNode();
    ~EntryNode() final = default;
protected:
    void drawContents() final;
private:
    OutPort mExecutionOutPort = OutPort(*this, "Out");
};

class RenderPassNode final : public Node {
public:
    RenderPassNode();
    ~RenderPassNode() override = default;
protected:
    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "In", {&typeid(EntryNode), &typeid(RenderPassNode)});
    OutPort mExecutionOutPort = OutPort(*this, "Out");
};

class ExitNode final : public Node {
public:
    ExitNode();
    ~ExitNode() final = default;
protected:
    void drawContents() final;
private:
    InPort mExecutionInPort = InPort(*this, "In", {&typeid(RenderPassNode)});
};
