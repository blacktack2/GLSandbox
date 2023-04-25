#pragma once
#include "BasicNodes.h"

#include "../../NodeEditor/Ports.h"

#include "../NodeClassifications.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <functional>

class UVNode final : public NumericNode<glm::vec2> {
public:
    UVNode();
    ~UVNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::UV;
    }
protected:
    bool drawInputArea(const std::string& label) final;
};

class ColourNode final : public NumericNode<glm::vec3> {
public:
    ColourNode();
    ~ColourNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Colour;
    }
protected:
    bool drawInputArea(const std::string& label) final;
};

class DirectionNode final : public NumericNode<glm::vec3> {
public:
    DirectionNode();
    ~DirectionNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Direction;
    }
protected:
    bool drawInputArea(const std::string& label) final;
};

class ModelMatrixNode final : public NumericNode<glm::mat4> {
public:
    ModelMatrixNode();
    ~ModelMatrixNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::ModelMatrix;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedDataExtra() const final;
    void deserializeDataExtra(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final {
        mValue = generateModelMatrix();
    }

    bool drawInputArea(const std::string& label) final;
private:
    [[nodiscard]] inline glm::mat4 generateModelMatrix() const {
        return glm::translate(mPosition) *
            glm::rotate(glm::radians(mRoll), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::rotate(glm::radians(mPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::radians(mYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::scale(mScale);
    }

    glm::vec3 mPosition = glm::vec3(0.0f);
    union {
        struct {
            float mRoll;
            float mPitch;
            float mYaw;
        };
        float mRotations[3] = { 0.0f, 0.0f, 0.0f };
    };
    glm::vec3 mScale = glm::vec3(1.0f);

    bool mShowData = false;
};

class ViewMatrixNode final : public NumericNode<glm::mat4> {
public:
    ViewMatrixNode();
    ~ViewMatrixNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::ViewMatrix;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedDataExtra() const final;
    void deserializeDataExtra(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final {
        mValue = generateViewMatrix();
    }

    bool drawInputArea(const std::string& label) final;
private:
    [[nodiscard]] inline glm::mat4 generateViewMatrix() const {
        return glm::rotate(-glm::radians(mRoll),  glm::vec3(0.0f, 0.0f, 1.0f)) *
               glm::rotate(-glm::radians(mPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
               glm::rotate(-glm::radians(mYaw),   glm::vec3(0.0f, 1.0f, 0.0f)) *
               glm::translate(-mPosition);
    }

    glm::vec3 mPosition = glm::vec3(0.0f);
    union {
        struct {
            float mRoll;
            float mPitch;
            float mYaw;
        };
        float mRotations[3] = { 0.0f, 0.0f, 0.0f };
    };

    bool mShowData = false;
};

class ProjMatrixNode final : public NumericNode<glm::mat4> {
public:
    ProjMatrixNode();
    ~ProjMatrixNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::ProjMatrix;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedDataExtra() const final;
    void deserializeDataExtra(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final {
        mValue = generateProjMatrix();
    }

    bool drawInputArea(const std::string& label) final;
private:
    enum class Type : std::size_t {
        Perspective = 0,
        Orthogonal,
        Max,
    };

    [[nodiscard]] inline glm::mat4 generateProjMatrix() const {
        switch (mType) {
            case Type::Perspective : return glm::perspective(glm::radians(mFoV), mAspect, mClipZ.x, mClipZ.y);
            case Type::Orthogonal  : return glm::ortho(mClipX.x, mClipX.y, mClipY.x, mClipY.y, mClipZ.x, mClipZ.y);
            default                : return glm::mat4(1.0f);
        }
    }

    static inline std::string getTypeName(Type type) {
        switch (type) {
            case Type::Perspective : return "Perspective";
            case Type::Orthogonal  : return "Orthogonal";
            default                : return "UNDEFINED";
        }
    }

    bool drawPerspective(const std::string& label);
    bool drawOrthogonal(const std::string& label);

    Type mType = Type::Perspective;

    float mFoV = 90.0f;
    float mAspect;
    glm::vec2 mClipX = glm::vec2(-100.0f, 100.0f);
    glm::vec2 mClipY = glm::vec2(-100.0f, 100.0f);
    glm::vec2 mClipZ = glm::vec2(1.0f, 100.0f);

    bool mShowData = false;
};
