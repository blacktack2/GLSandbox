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

class InPort;

class OutPort : public Port {
public:
    explicit OutPort(const Node& parent, const std::string& displayName, const std::any& value, int id = 0);

    void draw() const override;
    void drawLinks() const override;

    [[nodiscard]] inline const std::any& getValue() const {
        return mValue;
    }

    /**
     * @brief Two-way unlink port (both ports are set to be unlinked).
     */
    void unlink();
    /**
     * @brief Two-way link port (will link in->out and out->in). Existing links will be unlinked.
     * @param port Port to link to.
     */
    void link(InPort& port);
    /**
     * @brief Only unlink this port (to prevent infinite recursion during unlinking).
     */
    void unlinkSoft();
    /**
     * @brief Only link out->in (to prevent infinite recursion during linking).
     * @param port Port to link to.
     */
    void linkSoft(InPort& port);
private:
    InPort* mLink = nullptr;

    const std::any& mValue;
};

class InPort : public Port {
public:
    friend class OutPort;

    explicit InPort(const Node& parent, const std::string& displayName, int id = 0);

    void draw() const override;
    void drawLinks() const override;

    [[nodiscard]] inline const std::any& getValue() const {
        return mLink->getValue();
    }

    /**
     * @brief Two-way unlink port (both ports are set to be unlinked).
     */
    void unlink();
    /**
     * @brief Two-way link port (will link in->out and out->in). Existing links will be unlinked.
     * @param port Port to link to.
     */
    void link(OutPort& port);
    /**
     * @brief Only unlink this port (to prevent infinite recursion during unlinking).
     */
    void unlinkSoft();
    /**
     * @brief Only link in->out (to prevent infinite recursion during linking).
     * @param port Port to link to.
     */
    void linkSoft(OutPort& port);
private:
    OutPort* mLink = nullptr;

    int mLinkID = 0;
};
