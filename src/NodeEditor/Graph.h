#pragma once
#include "Node.h"

#include <memory>
#include <vector>

class Graph {
public:
    Graph();

    void draw();
protected:
    std::vector<std::unique_ptr<Node>> mNodes{};
};

