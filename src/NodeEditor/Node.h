#pragma once
#include "Ports.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

class Node {
public:
    void draw();

    [[nodiscard]] inline int getID() const {
        return mID;
    }
protected:
    explicit Node(std::string title, int id = 0);

    inline void addPort(const IPort& port) {
        mPorts.emplace_back(port);
    }

    virtual void drawContents() = 0;
private:
    int mID;

    std::string mTitle;

    std::vector<std::reference_wrapper<const IPort>> mPorts;
};

