#pragma once
#include "Node.h"

#include "../Rendering/Framebuffer.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/Shader.h"
#include "../Rendering/Texture.h"

#include "../Utils/ImUtils.h"
#include "../Utils/VariantUtils.h"

#include <glm/glm.hpp>

#include <imgui_node_editor.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_set>
#include <variant>

namespace ed = ax::NodeEditor;

class IPort {
public:
    typedef std::function<bool(IPort& linkTo)> validate_link_callback;
    typedef std::function<void()> on_link_callback;
    typedef std::function<void()> on_value_update_callback;
    typedef std::function<void()> on_unlink_callback;

    enum class Direction {
        In,
        Out,
    };

    IPort() = default;
    virtual ~IPort() = default;

    /**
     * @brief Callback used whenever a link is being attempted.
     * @param linkTo Port attempting to link.
     * @returns true if the link should be allowed, otherwise false to reject the link.
     */
    virtual void addValidateLinkEvent(const validate_link_callback& callback) = 0;
    /**
     * @brief Callback used whenever a link has been successfully created.
     */
    virtual void addOnLinkEvent(const on_link_callback& callback) = 0;
    /**
     * @brief Callback used whenever a value related to this port is updated, or when linked/unlinked.
     * @brief Specifically, this is called whenever IPort::valueUpdated() is called.
     */
    virtual void addOnUpdateEvent(const on_value_update_callback& callback) = 0;
    /**
     * @brief Callback used whenever an existing link has been broken.
     */
    virtual void addOnUnlinkEvent(const on_unlink_callback& callback) = 0;

    /**
     * @return true if all validation checks between this port and linkTo pass, otherwise false.
     * @see <a href="IPort::addValidateLinkEvent(const validate_link_callback& callback)">addValidateLinkEvent</a>
     */
    virtual bool validateLink(IPort& linkTo) = 0;

    /**
     * @brief Attempt to link this port with linkTo, or do nothing if a link cannot be generated.
     * @brief On success, this will create a two-way link with both ports referencing each other.
     * @param linkTo Port to attempt to link with.
     * @return true if a link has been successfully generated, otherwise false.
     */
    virtual bool link(IPort& linkTo) = 0;
    /**
     * @brief Link this port to linkTo, without affecting linkTo.
     * @param linkTo Port to link with.
     * @param linkID Unique ID of the link.
     */
    virtual void halfLink(IPort& linkTo, int linkID) = 0;
    /**
     * @brief Break any existing link, or do nothing if this port is not linked with another.
     * @brief If a link is present, this will cause a two-way break, such that both ports will no longer have any links.
     */
    virtual void unlink() = 0;
    /**
     * @brief Break any existing link.
     */
    virtual void halfUnlink() = 0;

    /**
     * @brief Call any update events in this port or a linked port, if present.
     * @see <a href="IPort::addOnUpdateEvent(const on_value_update_callback& callback)">addOnUpdateEvent</a>
     */
    virtual void valueUpdated() = 0;
    /**
     * @brief Call any update events in this port, without affecting any linked ports.
     */
    virtual void halfValueUpdated() = 0;

    [[nodiscard]] virtual bool isLinked() const = 0;
    [[nodiscard]] virtual bool isDynamic() const = 0;

    /**
     * @return Unique ID for this port.
     */
    [[nodiscard]] virtual int getID() const = 0;
    /**
     * @return Unique ID for this port's connection, or -1 if no connection is present.
     */
    [[nodiscard]] virtual int getLinkID() const = 0;
    /**
     * @return Unique ID of the port this port is connected to, or -1 if no connection is present.
     */
    [[nodiscard]] virtual int getLinkedPortID() const = 0;

    /**
     * @return Parent node to this port.
     */
    [[nodiscard]] virtual const Node& getParent() const = 0;
    /**
     * @return Parent node to the port this port is connected to. If no connection is present this will result in
     * undefined behaviour.
     */
    [[nodiscard]] virtual const Node& getLinkedParent() const = 0;
    /**
     * @return Parent node to this port.
     */
    [[nodiscard]] virtual Node& getParent() = 0;
    /**
     * @return Parent node to the port this port is connected to. If no connection is present this will result in
     * undefined behaviour.
     */
    [[nodiscard]] virtual Node& getLinkedParent() = 0;

    [[nodiscard]] virtual Direction getDirection() const = 0;

    [[nodiscard]] virtual const std::string& getUniqueName() const = 0;
    [[nodiscard]] virtual const std::string& getDisplayName() const = 0;

    virtual void drawPort() = 0;
    virtual void drawLink() = 0;

    [[nodiscard]] virtual std::set<std::type_index> getParameterTypes() const = 0;
};

template<typename... Ts>
std::size_t typeHash() {
    size_t result = 0;
    std::unordered_set<std::type_index> typeSet = { std::type_index(typeid(Ts))... };

    for (const auto& type : typeSet)
        result ^= type.hash_code();

    return result;
}

template<typename... Types>
class Port final : public IPort {
public:
    typedef std::function<std::variant<Types...>()> value_get_callback;

    /**
     * @brief Construct a new port.
     * @param parent Parent node which contains this port.
     * @param direction Whether this port should be considered an input or an output.
     * @param uniqueName Name of this port, unique within a given node.
     * @param displayName Text to be displayed next to the port pin.
     * @param getValue Callback for retrieving a value for this port. Should be left as nullptr for input nodes.
     * @param useDefault If true a default value will be generated and used.
     */
    Port(Node& parent, Direction direction, std::string uniqueName, std::string displayName,
         const value_get_callback& getValue = nullptr, bool useDefault = false, bool isDynamic = false) :
    mID(gGraphIDCounter++), mParent(parent),
    mDirection(direction), mUniqueName(std::move(uniqueName)), mDisplayName(std::move(displayName)),
    mGetValue(getValue), mUseDefault(useDefault), mIsDynamic(isDynamic) {
        generatePinData();
        switch (mDirection) {
            case Direction::In  : mDrawPort = [this]() { drawIn();  }; break;
            case Direction::Out : mDrawPort = [this]() { drawOut(); }; break;
        }
    }
    ~Port() final {
        if (mLink)
            mLink->halfUnlink();
    }

    void addValidateLinkEvent(const validate_link_callback& callback) final {
        mValidateLinks.emplace_back(callback);
    }
    void addOnLinkEvent(const on_link_callback& callback) final {
        mOnLinks.emplace_back(callback);
    }
    void addOnUpdateEvent(const on_value_update_callback& callback) final {
        mOnUpdates.emplace_back(callback);
    }
    void addOnUnlinkEvent(const on_unlink_callback& callback) final {
        mOnUnlinks.emplace_back(callback);
    }

    bool validateLink(IPort& linkTo) final {
        for (const auto& callback : mValidateLinks)
            if (!callback(linkTo))
                return false;
        return true;
    }

    bool link(IPort& linkTo) final {
        if (linkTo.getDirection() == getDirection() || !isTypeMatch(linkTo) ||
            !(validateLink(linkTo) && linkTo.validateLink(*this)))
            return false;

        if (mLink)
            unlink();
        int linkID = gGraphIDCounter++;
        halfLink(linkTo, linkID);
        linkTo.halfLink(*this, linkID);
        return true;
    }
    void halfLink(IPort& linkTo, int linkID) final {
        mLink = &linkTo;
        mLinkID = linkID;
        for (const auto& callback : mOnLinks)
            callback();
        valueUpdated();
    }
    void unlink() final {
        if (!mLink)
            return;
        mLink->halfUnlink();
        halfUnlink();
    }
    void halfUnlink() final {
        mLink = nullptr;
        for (const auto& callback : mOnUnlinks)
            callback();
        valueUpdated();
    }

    void valueUpdated() final {
        halfValueUpdated();
        if (mLink)
            mLink->halfValueUpdated();
    }
    void halfValueUpdated() final {
        for (const auto& callback : mOnUpdates)
            callback();
    }

    [[nodiscard]] bool isLinked() const final {
        return mLink != nullptr;
    }
    [[nodiscard]] bool isDynamic() const final {
        return mIsDynamic;
    }

    [[nodiscard]] int getID() const final {
        return mID;
    }
    [[nodiscard]] int getLinkID() const final {
        return mLinkID;
    }
    [[nodiscard]] int getLinkedPortID() const final {
        return mLink ? mLink->getID() : -1;
    }

    [[nodiscard]] const Node& getParent() const final {
        return mParent;
    }
    [[nodiscard]] const Node& getLinkedParent() const final {
        return mLink->getParent();
    }
    [[nodiscard]] Node& getParent() final {
        return mParent;
    }
    [[nodiscard]] Node& getLinkedParent() final {
        return mLink->getParent();
    }

    [[nodiscard]] Direction getDirection() const final {
        return mDirection;
    }

    [[nodiscard]] const std::string& getUniqueName() const final {
        return mUniqueName;
    }
    [[nodiscard]] const std::string& getDisplayName() const final {
        return mDisplayName;
    }

    void drawPort() final {
        mDrawPort();
    }
    void drawLink() final {
        if (mDirection == Direction::In && mLink)
            ed::Link(mLinkID, mLink->getID(), getID());
    }

    [[nodiscard]] std::set<std::type_index> getParameterTypes() const final {
        return { typeid(Types)... };
    };

    /**
     * @brief should only be called on input ports.
     */
    std::variant<Types...> getValue() const {
        return mGetValue();
    }
    std::variant<Types...> getConnectedValue() const {
        return mLink ? dynamic_cast<Port<Types...>*>(mLink)->getValue() : *mDefaultValue;
    }

    template<typename T>
    T getSingleValue() const {
        return std::visit([](const auto& arg)->T { return arg; }, mGetValue());
    }
    template<typename T>
    T getSingleConnectedValue() const {
        std::variant<Types...> value = mLink ? dynamic_cast<Port<Types...>*>(mLink)->getValue() : *mDefaultValue;
        return std::visit([](const auto& arg)->T { return arg; }, value);
    }
private:
    typedef std::function<void()> draw_port_callback;
    typedef std::function<void()> draw_pin_callback;

    enum class PinShape {
        Circle,
        FlatSquare,
        Square,
        Triangle,
        Arrow,
    };

    [[nodiscard]] bool isTypeMatch(IPort& other) const {
        const auto thisTypes = getParameterTypes();
        const auto otherTypes = other.getParameterTypes();
        switch (mDirection) {
            case Direction::In:
                return std::includes(otherTypes.begin(), otherTypes.end(), thisTypes.begin(), thisTypes.end());
            case Direction::Out:
                return std::includes(thisTypes.begin(), thisTypes.end(), otherTypes.begin(), otherTypes.end());
        }
        return false;
    }

    void drawIn() {
        ed::BeginPin(mID, ed::PinKind::Input);
        ed::PinPivotAlignment(ImVec2(0.0f, 0.5f));
        ed::PinPivotSize(ImVec2(0.0f, 0.0f));

        mDrawPin();
        ImGui::SameLine();
        ImGui::TextUnformatted(mDisplayName.c_str());

        ed::EndPin();
    }
    void drawOut() {
        ImGui::SameLine(mParent.getSize().x - ImGui::CalcTextSize(mDisplayName.c_str()).x - ImGui::GetStyle().FramePadding.x * 2.0f - ed::GetStyle().NodePadding.x - ed::GetStyle().NodePadding.z - mPinSize);
        ed::BeginPin(mID, ed::PinKind::Output);
        ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
        ed::PinPivotSize(ImVec2(0.0f, 0.0f));

        ImGui::TextUnformatted(mDisplayName.c_str());
        ImGui::SameLine();
        mDrawPin();

        ed::EndPin();
    }

    void generatePinCallback(PinShape shape, ImU32 colour) {
        float pinSize = mPinSize;
        switch (shape) {
            default:
            case PinShape::Circle:
                mDrawPin = [pinSize, colour]() {
                    ImUtils::Pins::circleIcon(pinSize, colour, ImColor(0.0f, 0.0f, 0.0f));
                };
                break;
            case PinShape::FlatSquare:
                mDrawPin = [pinSize, colour]() {
                    ImUtils::Pins::squareIcon(pinSize, colour, ImColor(0.0f, 0.0f, 0.0f));
                };
                break;
            case PinShape::Square:
                mDrawPin = [pinSize, colour]() {
                    ImUtils::Pins::nGonIcon(pinSize, 4, colour, ImColor(0.0f, 0.0f, 0.0f));
                };
                break;
            case PinShape::Triangle:
                mDrawPin = [pinSize, colour]() {
                    ImUtils::Pins::nGonIcon(pinSize, 3, colour, ImColor(0.0f, 0.0f, 0.0f));
                };
                break;
            case PinShape::Arrow:
                mDrawPin = [pinSize, colour]() {
                    ImUtils::Pins::arrowIcon(pinSize, colour, ImColor(0.0f, 0.0f, 0.0f));
                };
                break;
        }
    }

    void generatePinData() {
        size_t hash = typeHash<Types...>();
        if (hash == typeHash<void*>()) {
            mPinSize = 14.0f;
            generatePinCallback(PinShape::Arrow, ImColor(1.0f, 1.0f, 1.0f));
        } else if (hash == typeHash<int>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Circle, ImColor(1.0f, 1.0f, 0.0f));
        } else if (hash == typeHash<glm::ivec2>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Circle, ImColor(0.8f, 0.8f, 0.0f));
        } else if (hash == typeHash<glm::ivec3>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Circle, ImColor(0.6f, 0.6f, 0.0f));
        } else if (hash == typeHash<glm::ivec4>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Circle, ImColor(0.4f, 0.4f, 0.0f));
        } else if (hash == typeHash<float>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Circle, ImColor(0.0f, 1.0f, 1.0f));
        } else if (hash == typeHash<glm::vec2>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Triangle, ImColor(0.0f, 1.0f, 0.7f));
        } else if (hash == typeHash<glm::vec3>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Triangle, ImColor(0.0f, 1.0f, 0.4f));
        } else if (hash == typeHash<glm::vec4>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Triangle, ImColor(0.0f, 1.0f, 0.0f));
        } else if (hash == typeHash<glm::mat2>()) {
            mPinSize = 8.0f;
            generatePinCallback(PinShape::Square, ImColor(0.0f, 1.0f, 0.7f));
        } else if (hash == typeHash<glm::mat3>()) {
            mPinSize = 8.0f;
            generatePinCallback(PinShape::Square, ImColor(0.0f, 1.0f, 0.4f));
        } else if (hash == typeHash<glm::mat4>()) {
            mPinSize = 8.0f;
            generatePinCallback(PinShape::Square, ImColor(0.0f, 1.0f, 0.0f));
        } else if (hash == typeHash<Framebuffer*>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::FlatSquare, ImColor(1.0f, 1.0f, 1.0f));
        } else if (hash == typeHash<Mesh*>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::FlatSquare, ImColor(0.0f, 0.8f, 1.0f));
        } else if (hash == typeHash<Shader*>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::FlatSquare, ImColor(1.0f, 0.4f, 0.0f));
        } else if (hash == typeHash<Texture*>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::FlatSquare, ImColor(0.0f, 1.0f, 0.0f));
        } else if (hash == typeHash<float, int>()) {
            mPinSize = 10.0f;
            generatePinCallback(PinShape::Circle, ImColor(1.0f, 1.0f, 0.0f));
        } else {
            mPinSize = 8.0f;
            generatePinCallback(PinShape::Circle, ImColor(1.0f, 1.0f, 1.0f));
        }
    }

    value_get_callback mGetValue;
    draw_port_callback mDrawPort;
    draw_pin_callback mDrawPin;

    Node& mParent;

    int mID;

    Direction mDirection;

    std::string mDisplayName;
    std::string mUniqueName;

    bool mIsDynamic;
    bool mUseDefault;
    std::unique_ptr<std::variant<Types...>> mDefaultValue = nullptr;

    IPort* mLink = nullptr;
    int mLinkID = -1;

    std::vector<validate_link_callback> mValidateLinks;
    std::vector<on_link_callback> mOnLinks;
    std::vector<on_value_update_callback> mOnUpdates;
    std::vector<on_unlink_callback> mOnUnlinks;

    float mPinSize = 0.0f;
};
