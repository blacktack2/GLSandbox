#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/IPipelineHandler.h"

#include "../../Utils/VariantUtils.h"

#include "../NodeClassifications.h"

class Framebuffer;
class Mesh;
class Shader;
class Texture;

class EntryNode final : public Node {
public:
    explicit EntryNode(IPipelineHandler& pipelineHandler);
    ~EntryNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Entry;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    void pipelineUpdateEvent();
    void pipelineResetEvent();

    bool validatePipeline() const;
    void updatePipeline();

    IPipelineHandler& mPipelineHandler;

    Port<void*> mExecutionOut = Port<void*>(*this, IPort::Direction::Out, "Out", "Out", [&]() { return (void*)nullptr; });

    mutable std::string mMessage;
    mutable MessageType mMessageType = MessageType::Info;
};

class InputNode final : public Node {
public:
    enum class ValidationState {
        Valid,
        Unlinked,
    };

    typedef std::variant<
        int, glm::ivec2, glm::ivec3, glm::ivec4,
        float, glm::vec2, glm::vec3, glm::vec4,
        glm::mat2, glm::mat3, glm::mat4,
        Framebuffer*, Mesh*, Shader*, Texture*
    > input_t;
    typedef VariantCast<Port, input_t> port_t;

    InputNode();
    ~InputNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Input;
    }

    void drawInput();

    ValidationState isValid() {
        if (mDefaultIn.isLinked())
            return ValidationState::Valid;
        return ValidationState::Unlinked;
    }

    /**
     * @brief Set the value this node will output.
     * @brief value must match the specific type this node takes (see InputNode::getDefaultValue()).
     * @brief This node is assumed to be in a valid state (see InputNode::isValid()). Usage of this function in an
     * invalid state will result in undefined behaviour, and a likely segmentation fault.
     * @param value New value to use.
     */
    inline void setValue(input_t value) {
        mExternalInput = std::make_unique<input_t>(value);
    }
    /**
     * @brief Remove any externally set values (see InputNode::setValue()), and use the default value instead.
     */
    inline void unsetValue() {
        mExternalInput = nullptr;
    }
    /**
     * @brief This node is assumed to be ina valid state (see InputNode::isValid()). Usage of this function in an
     * invalid state will result in undefined behaviour, and a likely segmentation fault.
     * @return Default value for this node, used when no external value is set.
     */
    inline input_t getDefaultValue() {
        return mDefaultIn.getValue();
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    std::unique_ptr<input_t> mExternalInput = nullptr;

    port_t mDefaultIn = port_t(*this, IPort::Direction::In, "DefaultIn", "Default");
    std::unique_ptr<IPort> mValueOut;
};

class OutputNode final : public Node {
public:
    enum class ValidationState {
        Valid,
        Unlinked,
    };

    typedef std::variant<
        int, glm::ivec2, glm::ivec3, glm::ivec4,
        float, glm::vec2, glm::vec3, glm::vec4,
        glm::mat2, glm::mat3, glm::mat4,
        Framebuffer*, Mesh*, Shader*, Texture*
    > input_t;
    typedef VariantCast<Port, input_t> port_t;

    OutputNode();
    ~OutputNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Output;
    }

    void drawOutput();

    ValidationState isValid() {
        if (mValueIn.isLinked())
            return ValidationState::Valid;
        return ValidationState::Unlinked;
    }

    template<typename T>
    T getValue() {
        return std::visit(VisitOverload{
            [](const auto& arg) { return T{}; },
            [](const T& arg) { return arg; },
        }, mValueIn.getValue());
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() final;
private:
    port_t mValueIn = port_t(*this, IPort::Direction::In, "ValueIn", "Value");
};
