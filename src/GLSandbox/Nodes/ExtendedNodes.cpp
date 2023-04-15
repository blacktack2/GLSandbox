#include "ExtendedNodes.h"

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
    return ImUtils::inputFloatN(&mValue[0], 2, label, -1.0f, 1.0f);
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
    return ImUtils::inputFloatN(&mValue[0], 3, label, 0.0f, 1.0f);
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
    if (ImUtils::inputFloatN(&mValue[0], 3, label, -1.0f, 1.0f)) {
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
    std::stringstream scaleStream;
    scaleStream    << mScale.x    << " " << mScale.y    << " " << mScale.z;
    return {
        {"Position", positionStream.str()},
        {"Roll",     std::to_string(mRoll)},
        {"Pitch",    std::to_string(mPitch)},
        {"Yaw",      std::to_string(mYaw)},
        {"Scale",    scaleStream.str()   },
    };
}

void ModelMatrixNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Position") {
        stream >> mPosition.x >> mPosition.y >> mPosition.z;
    } else if (dataID == "Roll") {
        stream >> mRoll;
    } else if (dataID == "Pitch") {
        stream >> mPitch;
    } else if (dataID == "Yaw") {
        stream >> mYaw;
    } else if (dataID == "Scale") {
        stream >> mScale.x    >> mScale.y    >> mScale.z;
    }
}

bool ModelMatrixNode::drawInputArea(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("Position");
    valueUpdated |= ImUtils::inputFloatN(&mPosition[0], 3, std::string(label).append("Position"));

    ImUtils::multiInputLabel("Roll", "Pitch", "Yaw");
    valueUpdated |= ImUtils::inputFloatN(&mRotations[0], 3, std::string(label).append("Rotation"), 0.0f, 360.0f);

    ImGui::Text("Scale");
    valueUpdated |= ImUtils::inputFloatN(&mScale[0], 3, std::string(label).append("Scale"), 0.0f, FLT_MAX);

    if (valueUpdated) {
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
    return {
        {"Position", positionStream.str()},
        {"Roll",     std::to_string(mRoll)},
        {"Pitch",    std::to_string(mPitch)},
        {"Yaw",      std::to_string(mYaw)},
    };
}

void ViewMatrixNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Position") {
        stream >> mPosition.x >> mPosition.y >> mPosition.z;
    } else if (dataID == "Roll") {
        stream >> mRoll;
    } else if (dataID == "Pitch") {
        stream >> mPitch;
    } else if (dataID == "Yaw") {
        stream >> mYaw;
    }
}

bool ViewMatrixNode::drawInputArea(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("Position");
    valueUpdated |= ImUtils::inputFloatN(&mPosition[0], 3, std::string(label).append("Position"));

    ImUtils::multiInputLabel("Roll", "Pitch", "Yaw");
    valueUpdated |= ImUtils::inputFloatN(&mRotations[0], 3, std::string(label).append("Rotation"), 0.0f, 360.0f);

    if (valueUpdated) {
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
    bool valueUpdated = false;

    ImUtils::cycleButton(std::string(label).append("Type"), (size_t&)mType, (size_t)Type::Max,
                         [](size_t index) { return getTypeName((Type)index); });

    switch (mType) {
        default: break;
        case Type::Perspective : valueUpdated = drawPerspective(label) || valueUpdated; break;
        case Type::Orthogonal  : valueUpdated = drawOrthogonal(label)  || valueUpdated; break;
    }

    if (valueUpdated) {
        mValue = generateProjMatrix();
        return true;
    }
    return false;
}

bool ProjMatrixNode::drawPerspective(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("FoV");
    valueUpdated |= ImUtils::inputFloat(&mFoV, std::string(label).append("FoV"), 0.0f, 180.0f);

    ImGui::Text("Aspect Ratio");
    valueUpdated |= ImUtils::inputFloat(&mAspect, std::string(label).append("Aspect"), 0.0f, FLT_MAX);

    ImUtils::multiInputLabel("Near", "Far");
    valueUpdated |= ImUtils::inputFloatN(&mClipZ[0], 2, std::string(label).append("NF"), 0.0f, FLT_MAX);

    return valueUpdated;
}

bool ProjMatrixNode::drawOrthogonal(const std::string& label) {
    bool valueUpdated = false;

    ImUtils::multiInputLabel("Left", "Right");
    valueUpdated |= ImUtils::inputFloatN(&mClipX[0], 2, std::string(label).append("LR"));

    ImUtils::multiInputLabel("Top", "Bottom");
    valueUpdated |= ImUtils::inputFloatN(&mClipY[0], 2, std::string(label).append("TB"));

    ImUtils::multiInputLabel("Near", "Far");
    valueUpdated |= ImUtils::inputFloatN(&mClipZ[0], 2, std::string(label).append("NF"));

    return valueUpdated;
}
