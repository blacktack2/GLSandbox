#pragma once
#include <any>
#include <functional>
#include <optional>
#include <string>
#include <typeinfo>
#include <vector>

class Node;

class IPort {
public:
    typedef std::function<void()> on_link_callback;

    virtual ~IPort() = default;

    virtual void draw() const = 0;
    virtual void drawLinks() const = 0;

    virtual void addOnLinkCallback(const on_link_callback& callback) = 0;

    [[nodiscard]] virtual int getID() const = 0;

    [[nodiscard]] virtual const Node& getParent() const = 0;
    [[nodiscard]] virtual std::any getLinkValue() const = 0;
    [[nodiscard]] virtual bool isLinked() const = 0;

    [[nodiscard]] virtual const std::string& getDisplayName() const = 0;

    virtual void unlink() = 0;
    [[nodiscard]] virtual int getLinkID() const = 0;
protected:
    IPort() = default;
};

class Port : public IPort {
public:
    ~Port() override = default;

    void addOnLinkCallback(const on_link_callback& callback) final {
        mOnLinks.emplace_back(callback);
    }

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
    Port(const Node& parent, std::string displayName);

    inline void onLink() {
        for (const auto& callback : mOnLinks)
            callback();
    }
private:
    int mID;

    std::string mDisplayName;

    const Node& mParentNode;

    std::vector<on_link_callback> mOnLinks{};
};

class InPort;

class OutPort final : public Port {
public:
    typedef std::function<std::any()> get_node_value_callback;

    OutPort(const Node& parent, const std::string& displayName, get_node_value_callback getValue);
    ~OutPort() final = default;

    void draw() const override;
    void drawLinks() const override;

    [[nodiscard]] std::any getLinkValue() const final;
    [[nodiscard]] bool isLinked() const final;

    [[nodiscard]] int getLinkID() const override;

    /**
     * @brief Two-way unlink port (both ports are set to be unlinked).
     */
    void unlink() final;
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

    [[nodiscard]] const Node& getLinkParent() const;
private:
    InPort* mLink = nullptr;

    get_node_value_callback mGetValue;
};

class InPort final : public Port {
public:
    explicit InPort(const Node& parent, const std::string& displayName,
                    std::vector<const std::type_info*> validConnections = {});
    ~InPort() final = default;

    void draw() const final;
    void drawLinks() const final;

    [[nodiscard]] std::any getLinkValue() const final;
    [[nodiscard]] bool isLinked() const final;

    [[nodiscard]] int getLinkID() const final;
    [[nodiscard]] int getLinkedPortID() const;

    /**
     * @brief Two-way unlink port (both ports are set to be unlinked).
     */
    void unlink() override;
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
