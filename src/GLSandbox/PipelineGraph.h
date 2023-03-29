#pragma once
#include "../NodeEditor/Graph.h"

class PipelineGraph : public Graph {
public:
    PipelineGraph();
protected:
    void drawNodeCreation() override;
};

