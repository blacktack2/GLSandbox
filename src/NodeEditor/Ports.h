#pragma once
#include <any>
#include <functional>
#include <optional>
#include <string>

class Node;

class IPort {
public:
    virtual void draw() const = 0;
    virtual void drawLinks() const = 0;

    [[nodiscard]] virtual int getID() const = 0;

    [[nodiscard]] virtual const std::string& getDisplayName() const = 0;
};

class Port : public IPort {
public:
    [[nodiscard]] int getID() const override {
        return mID;
    }

    [[nodiscard]] const std::string& getDisplayName() const override {
        return mDisplayName;
    }

    [[nodiscard]] const Node& getParentNode() const {
        return mParentNode;
    }
protected:
    explicit Port(const Node& parent, std::string displayName, int id = 0);
private:
    int mID;

    std::string mDisplayName;

    const Node& mParentNode;
};

class OutPort : public Port {
public:
    explicit OutPort(const Node& parent, const std::string& displayName, const std::any& value, int id = 0);

    void draw() const override;
    void drawLinks() const override;

    [[nodiscard]] inline const std::any& getValue() const {
        return mValue;
    }
private:
    const std::any& mValue;
};

class InPort : public Port {
public:
    explicit InPort(const Node& parent, const std::string& displayName, int id = 0);

    void draw() const override;
    void drawLinks() const override;

    [[nodiscard]] inline const std::any& getValue() const {
        return mLink->getValue();
    }
    inline void unlink() {
        mLink = nullptr;
    }
    inline void link(const OutPort& port) {
        static int cLinkIDCounter = 1;
        unlink();
        mLink = &port;
        mLinkID = cLinkIDCounter++;
    }
private:
    const OutPort* mLink = nullptr;

    int mLinkID = 0;
};
