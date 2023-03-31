#pragma once
#include <functional>
#include <optional>
#include <string>
#include <typeinfo>
#include <vector>

class Node;

class IPort {
public:
    virtual ~IPort() = default;

    virtual void draw() const = 0;
    virtual void drawLinks() const = 0;

    [[nodiscard]] virtual int getID() const = 0;

    [[nodiscard]] virtual const Node& getParent() const = 0;

    [[nodiscard]] virtual const std::string& getDisplayName() const = 0;
};

class Port : public IPort {
public:
    ~Port() override = default;

    [[nodiscard]] int getID() const final {
        return mID;
    }

    [[nodiscard]] const std::string& getDisplayName() const final {
        return mDisplayName;
    }

    [[nodiscard]] const Node& getParent() const final {
        return mParentNode;
    }
protected:
    Port(const Node& parent, std::string displayName, int id = 0);
private:
    int mID;

    std::string mDisplayName;

    const Node& mParentNode;
};

class InPort;

class OutPort final : public Port {
public:
    OutPort(const Node& parent, const std::string& displayName, int id = 0);
    ~OutPort() final = default;

    void draw() const override;
    void drawLinks() const override;

    /**
     * @brief Two-way unlink port (both ports are set to be unlinked).
     */
    void unlink();
    /**
     * @brief Two-way link port (will link in->out and out->in). Existing links will be unlinked.
     * @brief If this is not a valid link for port, as defined by validConnections (see InPort::InPort()), no link will
     * be created.
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
};

class InPort final : public Port {
public:
    explicit InPort(const Node& parent, const std::string& displayName,
                    std::vector<const std::type_info*> validConnections = {}, int id = 0);
    ~InPort() final = default;

    void draw() const override;
    void drawLinks() const override;

    /**
     * @brief Two-way unlink port (both ports are set to be unlinked).
     */
    void unlink();
    /**
     * @brief Two-way link port (will link in->out and out->in). Existing links will be unlinked.
     * @brief If port is not a valid link, as defined by validConnections (see InPort::InPort()), no link will be
     * created.
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

    bool isConnectionValid(const IPort& port);
private:
    OutPort* mLink = nullptr;

    int mLinkID = 0;

    std::vector<const std::type_info*> mValidConnections;
};
