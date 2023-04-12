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

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::UV;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class ColourNode final : public NumericNode<glm::vec3> {
public:
    ColourNode();
    ~ColourNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Colour;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class DirectionNode final : public NumericNode<glm::vec3> {
public:
    DirectionNode();
    ~DirectionNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Direction;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
};

class ModelMatrixNode final : public NumericNode<glm::mat4> {
public:
    ModelMatrixNode();
    ~ModelMatrixNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::ModelMatrix;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
private:
    inline glm::mat4 generateModelMatrix() {
        return glm::translate(mPosition) *
            glm::rotate(glm::radians(mRoll), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::rotate(glm::radians(mPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::radians(mYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::scale(mScale);
    }

    glm::vec3 mPosition = glm::vec3(0.0f);
    float mRoll = 0.0f;
    float mPitch = 0.0f;
    float mYaw = 0.0f;
    glm::vec3 mScale = glm::vec3(1.0f);
};

class ViewMatrixNode final : public NumericNode<glm::mat4> {
public:
    ViewMatrixNode();
    ~ViewMatrixNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::ViewMatrix;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
private:
    inline glm::mat4 generateViewMatrix() {
        return glm::rotate(-glm::radians(mRoll),  glm::vec3(0.0f, 0.0f, 1.0f)) *
               glm::rotate(-glm::radians(mPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
               glm::rotate(-glm::radians(mYaw),   glm::vec3(0.0f, 1.0f, 0.0f)) *
               glm::translate(-mPosition);
    }

    glm::vec3 mPosition = glm::vec3(0.0f);
    float mRoll  = 0.0f;
    float mPitch = 0.0f;
    float mYaw   = 0.0f;
};

class ProjMatrixNode final : public NumericNode<glm::mat4> {
public:
    ProjMatrixNode();
    ~ProjMatrixNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::ProjMatrix;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    bool drawInputArea(const std::string& label) final;
private:
    enum class Type {
        Perspective,
        Orthogonal,
        Max,
    };

    inline glm::mat4 generateProjMatrix() {
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
    float mAspect = 1.0f;
    glm::vec2 mClipX = glm::vec2(1.0f, 100.0f);
    glm::vec2 mClipY = glm::vec2(1.0f, 100.0f);
    glm::vec2 mClipZ = glm::vec2(1.0f, 100.0f);
};
