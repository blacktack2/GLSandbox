#pragma once
#include <any>
#include <functional>
#include <optional>
#include <string>

class IPort {
public:
    virtual void draw() const = 0;
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
protected:
    explicit Port(std::string displayName, int id = 0);
private:
    int mID;

    std::string mDisplayName;
};

class OutPort : public Port {
public:
    explicit OutPort(const std::string& displayName, const std::any& value, int id = 0);

    void draw() const override;

    [[nodiscard]] inline const std::any& getValue() const {
        return mValue;
    }
private:
    const std::any& mValue;
};

class InPort : public Port {
public:
    explicit InPort(const std::string& displayName, int id = 0);

    void draw() const override;

    [[nodiscard]] inline const std::any& getValue() const {
        return mLink.value().get().getValue();
    }
    inline void unlink() {
        mLink = std::optional<std::reference_wrapper<const OutPort>>();
    }
    inline void link(const OutPort& port) {
        static int cLinkIDCounter = 1;
        unlink();
        mLink = port;
        mLinkID = cLinkIDCounter++;
    }
private:
    std::optional<std::reference_wrapper<const OutPort>> mLink;

    int mLinkID = 0;
};
