#include "ExtendedNodes.h"

#include <imgui.h>

#include <sstream>

UVNode::UVNode() : NumericNode<glm::vec2>("UV") {
    mValue = glm::vec2(0.0f);
}

std::vector<std::pair<std::string, std::string>> UVNode::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue.x << " " << mValue.y;
    return {
        {"Value", stream.str()}
    };
}

void UVNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue.x >> mValue.y;
}

bool UVNode::drawInputArea(const std::string& label) {
    return ImGui::DragFloat2(label.c_str(), &mValue[0], 0.01f, -1.0f, 1.0f, "%.2f");
}

ColourNode::ColourNode() : NumericNode<glm::vec3>("Colour") {
    mValue = glm::vec3(1.0f);
}

std::vector<std::pair<std::string, std::string>> ColourNode::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue.r << " " << mValue.g << " " << mValue.b;
    return {
        {"Value", stream.str()}
    };
}

void ColourNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue.r >> mValue.g >> mValue.b;
}

bool ColourNode::drawInputArea(const std::string& label) {
    return ImGui::DragFloat3(label.c_str(), &mValue[0], 0.01f, 0.0f, 1.0f, "%.2f");
}

DirectionNode::DirectionNode() : NumericNode<glm::vec3>("Direction") {
    mValue = glm::vec3(0.0f, 1.0f, 0.0f);
}

std::vector<std::pair<std::string, std::string>> DirectionNode::generateSerializedData() const {
    std::stringstream stream;
    stream << mValue.x << " " << mValue.y << " " << mValue.z;
    return {
        {"Value", stream.str()}
    };
}

void DirectionNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Value")
        stream >> mValue.x >> mValue.y >> mValue.z;
}

bool DirectionNode::drawInputArea(const std::string& label) {
    glm::vec3 tempValue = mValue;
    if (ImGui::DragFloat3(label.c_str(), &tempValue[0], 0.01f, -1.0f, 1.0f, "%.2f")) {
        if (tempValue.x != mValue.x) {
            float x = tempValue.x;
            tempValue = glm::normalize(tempValue);

            float squareSum = (tempValue.y * tempValue.y + tempValue.z * tempValue.z);
            if (squareSum != 0) {
                float scale = std::sqrt((1.0f - x * x) / squareSum);
                mValue = glm::vec3(x, tempValue.y * scale, tempValue.z * scale);
            } else {
                float delta = 1 - x;
                mValue = glm::vec3(x, delta * 0.5f * (tempValue.y < 0.0f ? -1.0f : 1.0f), delta * 0.5f * (tempValue.z < 0.0f ? -1.0f : 1.0f));
            }
        } else if (tempValue.y != mValue.y) {
            float y = tempValue.y;
            tempValue = glm::normalize(tempValue);

            float squareSum = (tempValue.x * tempValue.x + tempValue.z * tempValue.z);
            if (squareSum != 0) {
                float scale = std::sqrt((1.0f - y * y) / squareSum);
                mValue = glm::vec3(tempValue.x * scale, y, tempValue.z * scale);
            } else {
                float delta = 1 - y;
                mValue = glm::vec3(delta * 0.5f * (tempValue.x < 0.0f ? -1.0f : 1.0f), y, delta * 0.5f * (tempValue.z < 0.0f ? -1.0f : 1.0f));
            }
        } else if (tempValue.z != mValue.z) {
            float z = tempValue.z;
            tempValue = glm::normalize(tempValue);

            float squareSum = (tempValue.x * tempValue.x + tempValue.y * tempValue.y);
            if (squareSum != 0) {
                float scale = std::sqrt((1.0f - z * z) / squareSum);
                mValue = glm::vec3(tempValue.x * scale, tempValue.y * scale, z);
            } else {
                float delta = 1 - z;
                mValue = glm::vec3(delta * 0.5f * (tempValue.x < 0.0f ? -1.0f : 1.0f), delta * 0.5f * (tempValue.y < 0.0f ? -1.0f : 1.0f), z);
            }
        }
        return true;
    }
    return false;
}

ModelMatrixNode::ModelMatrixNode() : NumericNode<glm::mat4>("Model") {
    mValue = generateModelMatrix();
}

std::vector<std::pair<std::string, std::string>> ModelMatrixNode::generateSerializedData() const {
    std::stringstream positionStream;
    positionStream << mPosition.x << " " << mPosition.y << " " << mPosition.z;
    std::stringstream rotationStream;
    rotationStream << mRotation.x << " " << mRotation.y << " " << mRotation.z << " " << mRotation.w;
    std::stringstream scaleStream;
    scaleStream    << mScale.x    << " " << mScale.y    << " " << mScale.z;
    return {
        {"Position", positionStream.str()},
        {"Rotation", rotationStream.str()},
        {"Scale",    scaleStream.str()   },
    };
}

void ModelMatrixNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Position") {
        stream >> mPosition.x >> mPosition.y >> mPosition.z;
    } else if (dataID == "Rotation") {
        stream >> mRotation.x >> mRotation.y >> mRotation.z >> mRotation.w;
    } else if (dataID == "Scale") {
        stream >> mScale.x    >> mScale.y    >> mScale.z;
    }
}

bool ModelMatrixNode::drawInputArea(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("Position");
    valueUpdated = ImGui::InputFloat3(std::string(label).append("Position").c_str(), &mPosition[0]);

    ImGui::Text("Rotation");
    valueUpdated = valueUpdated || ImGui::DragFloat(std::string(label).append("Rotation").c_str(), &mRotation[0], 0.01f, -1.0f, 1.0f);
    ImGui::SameLine();
    ImGui::Text("Angle");
    ImGui::SameLine();
    valueUpdated = valueUpdated || ImGui::InputFloat(std::string(label).append("Angle").c_str(), &mRotation.w);

    ImGui::Text("Scale");
    valueUpdated = valueUpdated || ImGui::InputFloat3(std::string(label).append("Scale").c_str(), &mScale[0]);

    if (valueUpdated) {
        mRotation = glm::vec4(glm::normalize(glm::vec3(mRotation)), mRotation.w);
        mValue = generateModelMatrix();
        return true;
    }
    return false;
}

ViewMatrixNode::ViewMatrixNode() : NumericNode<glm::mat4>("View") {
    mValue = generateViewMatrix();
}

std::vector<std::pair<std::string, std::string>> ViewMatrixNode::generateSerializedData() const {
    std::stringstream positionStream;
    positionStream << mPosition.x << " " << mPosition.y << " " << mPosition.z;
    std::stringstream rotationStream;
    rotationStream << mRotation.x << " " << mRotation.y << " " << mRotation.z << " " << mRotation.w;
    std::stringstream scaleStream;
    scaleStream    << mScale.x    << " " << mScale.y    << " " << mScale.z;
    return {
        {"Position", positionStream.str()},
        {"Rotation", rotationStream.str()},
        {"Scale",    scaleStream.str()   },
    };
}

void ViewMatrixNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Position") {
        stream >> mPosition.x >> mPosition.y >> mPosition.z;
    } else if (dataID == "Rotation") {
        stream >> mRotation.x >> mRotation.y >> mRotation.z >> mRotation.w;
    } else if (dataID == "Scale") {
        stream >> mScale.x    >> mScale.y    >> mScale.z;
    }
}

bool ViewMatrixNode::drawInputArea(const std::string& label) {
    bool valueUpdated;

    ImGui::Text("Position");
    valueUpdated = ImGui::InputFloat3(std::string(label).append("Position").c_str(), &mPosition[0]);

    ImGui::Text("Rotation");
    valueUpdated = ImGui::DragFloat(std::string(label).append("Rotation").c_str(), &mRotation[0], 0.01f, -1.0f, 1.0f) || valueUpdated;
    ImGui::SameLine();
    ImGui::Text("Angle");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Angle").c_str(), &mRotation.w) || valueUpdated;

    ImGui::Text("Scale");
    valueUpdated = ImGui::InputFloat3(std::string(label).append("Scale").c_str(), &mScale[0]) || valueUpdated;

    if (valueUpdated) {
        mRotation = glm::vec4(glm::normalize(glm::vec3(mRotation)), mRotation.w);
        mValue = generateViewMatrix();
        return true;
    }
    return false;
}

ProjMatrixNode::ProjMatrixNode() : NumericNode<glm::mat4>("Projection") {
    mValue = generateProjMatrix();
}

std::vector<std::pair<std::string, std::string>> ProjMatrixNode::generateSerializedData() const {
    std::stringstream clipXStream;
    clipXStream << mClipX.x << " " << mClipX.y;
    std::stringstream clipYStream;
    clipYStream << mClipY.x << " " << mClipY.y;
    std::stringstream clipZStream;
    clipZStream << mClipZ.x << " " << mClipZ.y;
    return {
        {"Type",   std::to_string((unsigned int)mType)},
        {"FoV",    std::to_string(mFoV)               },
        {"Aspect", std::to_string(mAspect)            },
        {"ClipX",  clipXStream.str()                  },
        {"ClipY",  clipYStream.str()                  },
        {"ClipZ",  clipZStream.str()                  },
    };
}

void ProjMatrixNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Type") {
        unsigned int type;
        stream >> type;
        mType = (Type)type;
    } else if (dataID == "FoV") {
        stream >> mFoV;
    } else if (dataID == "Aspect") {
        stream >> mAspect;
    } else if (dataID == "ClipX") {
        stream >> mClipX.x >> mClipX.y;
    } else if (dataID == "ClipY") {
        stream >> mClipY.x >> mClipY.y;
    } else if (dataID == "ClipZ") {
        stream >> mClipZ.x >> mClipZ.y;
    }
}

bool ProjMatrixNode::drawInputArea(const std::string& label) {
    if (ImGui::BeginCombo(std::string(label).append("Combo").c_str(), getTypeName(mType).c_str())) {
        for (unsigned int i = 0; i < (unsigned int)Type::Max; i++) {
            Type type = (Type)i;
            bool isSelected = mType == type;
            if (ImGui::Selectable(getTypeName(type).c_str(), isSelected))
                mType = type;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    switch (mType) {
        case Type::Perspective : return drawPerspective(label);
        case Type::Orthogonal  : return drawOrthogonal(label);
        default                : return false;
    }
}

bool ProjMatrixNode::drawPerspective(const std::string& label) {
    bool valueUpdated;

    ImGui::Text("FoV");
    valueUpdated = ImGui::InputFloat(std::string(label).append("FoV").c_str(), &mFoV);

    ImGui::Text("Aspect Ratio");
    valueUpdated = ImGui::InputFloat(std::string(label).append("Aspect").c_str(), &mAspect) || valueUpdated;

    ImGui::Text("Near:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Near").c_str(), &mClipZ.x) || valueUpdated;
    ImGui::SameLine();
    ImGui::Text("Far:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Far").c_str(), &mClipZ.y) || valueUpdated;

    if (valueUpdated) {
        mFoV = std::min(std::max(mFoV, 0.0f), 179.0f);
        mValue = generateProjMatrix();
        return true;
    }
    return false;
}

bool ProjMatrixNode::drawOrthogonal(const std::string& label) {
    bool valueUpdated;

    ImGui::Text("Left:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Left").c_str(), &mClipX.x);
    ImGui::SameLine();
    ImGui::Text("Right:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Right").c_str(), &mClipX.y) || valueUpdated;

    ImGui::Text("Top:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Top").c_str(), &mClipY.x) || valueUpdated;
    ImGui::SameLine();
    ImGui::Text("Bottom:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Bottom").c_str(), &mClipY.y) || valueUpdated;

    ImGui::Text("Near:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Near").c_str(), &mClipZ.x) || valueUpdated;
    ImGui::SameLine();
    ImGui::Text("Far:");
    ImGui::SameLine();
    valueUpdated = ImGui::InputFloat(std::string(label).append("Far").c_str(), &mClipZ.y) || valueUpdated;

    if (valueUpdated) {
        mValue = generateProjMatrix();
        return true;
    }
    return false;
}
