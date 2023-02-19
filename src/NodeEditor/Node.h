#pragma once
#include <string>
#include <vector>

class Node;

class Connection {
public:
    Connection(Node& from, Node& to);

    inline Node& from() {
        return mFromNode;
    }
    inline Node& to() {
        return mToNode;
    }
private:
    Node& mFromNode;
    Node& mToNode;
};

class Port {
public:
    Port();

    void draw();
};

class InPort : public Port {
public:
    InPort();
};

class OutPort : public Port {
public:
    OutPort();
};

class Node {
public:
    Node(std::string title, int id = 0);

    void draw();
    void execute();

    inline int getID() {
        return mID;
    }
protected:
    virtual void drawInput() = 0;
private:
    int mID;

    std::string mTitle;

    std::vector<InPort> mInPorts;
    std::vector<OutPort> mOutPorts;
};

