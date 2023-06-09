#include "ExtendedNodes.h"

#include "../../Rendering/RenderConfig.h"

#include <sstream>

UVNode::UVNode() : NumericNode<glm::vec2>("UV") {
    mValue = glm::vec2(0.0f);
}

bool UVNode::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 2, label, -1.0f, 1.0f);
}

ColourNode::ColourNode() : NumericNode<glm::vec3>("Colour") {
    mValue = glm::vec3(1.0f);
}

bool ColourNode::drawInputArea(const std::string& label) {
    return ImUtils::inputFloatN(&mValue[0], 3, label, 0.0f, 1.0f);
}

DirectionNode::DirectionNode() : NumericNode<glm::vec3>("Direction") {
    mValue = glm::vec3(0.0f, 1.0f, 0.0f);
}

void fixedNormalize(float fixed, float& a, float& b) {
    glm::vec3 normalVec = glm::normalize(glm::vec3(fixed, a, b));

    float squareSum = (normalVec.y * normalVec.y) + (normalVec.z * normalVec.z);
    if (squareSum == 0) {
        float delta = 1 - fixed;
        a = delta * 0.5f * (normalVec.y < 0.0f ? -1.0f : 1.0f);
        b = delta * 0.5f * (normalVec.z < 0.0f ? -1.0f : 1.0f);
    } else {
        float scale = std::sqrt((1.0f - fixed * fixed) / squareSum);
        a = normalVec.y * scale;
        b = normalVec.z * scale;
    }
}

bool DirectionNode::drawInputArea(const std::string& label) {
    glm::vec3 oldValue = mValue;
    if (ImUtils::inputFloatN(&mValue[0], 3, label, -1.0f, 1.0f)) {
        if (oldValue.x != mValue.x)
            fixedNormalize(mValue.x, mValue.y, mValue.z);
        else if (oldValue.y != mValue.y)
            fixedNormalize(mValue.y, mValue.x, mValue.z);
        else if (oldValue.z != mValue.z)
            fixedNormalize(mValue.z, mValue.x, mValue.y);
        return true;
    }
    return false;
}

ModelMatrixNode::ModelMatrixNode() : NumericNode<glm::mat4>("Model") {
    mValue = generateModelMatrix();
}

std::vector<std::pair<std::string, std::string>> ModelMatrixNode::generateSerializedDataExtra() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Position", SerializationUtils::serializeData(mPosition));
    data.emplace_back("Roll", SerializationUtils::serializeData(mRoll));
    data.emplace_back("Pitch", SerializationUtils::serializeData(mPitch));
    data.emplace_back("Yaw", SerializationUtils::serializeData(mYaw));
    data.emplace_back("Scale", SerializationUtils::serializeData(mScale));
    return data;
}

void ModelMatrixNode::deserializeDataExtra(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Position")
        SerializationUtils::deserializeData(stream, mPosition);
    else if (dataID == "Roll")
        SerializationUtils::deserializeData(stream, mRoll);
    else if (dataID == "Pitch")
        SerializationUtils::deserializeData(stream, mPitch);
    else if (dataID == "Yaw")
        SerializationUtils::deserializeData(stream, mYaw);
    else if (dataID == "Scale")
        SerializationUtils::deserializeData(stream, mScale);
}

bool ModelMatrixNode::drawInputArea(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("Position");
    valueUpdated |= ImUtils::inputFloatN(&mPosition[0], 3, std::string(label).append("Position"));

    ImUtils::multiInputLabel("Roll", "Pitch", "Yaw");
    valueUpdated |= ImUtils::inputFloatN(&mRotations[0], 3, std::string(label).append("Rotation"), 0.0f, 360.0f);

    ImGui::Text("Scale");
    valueUpdated |= ImUtils::inputFloatN(&mScale[0], 3, std::string(label).append("Scale"), 0.0f, FLT_MAX);

    if (valueUpdated)
        mValue = generateModelMatrix();

    if (ImUtils::beginHeader("Data", generateNodeLabelID("DataHeader"), mShowData)) {
        ImGui::BeginDisabled();
        ImUtils::inputFloatNxN(&mValue[0][0], 4, generateNodeLabelID("MatrixData"));
        ImGui::EndDisabled();
        ImUtils::endHeader();
    }

    return valueUpdated;
}

ViewMatrixNode::ViewMatrixNode() : NumericNode<glm::mat4>("View") {
    mValue = generateViewMatrix();
}

std::vector<std::pair<std::string, std::string>> ViewMatrixNode::generateSerializedDataExtra() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Position", SerializationUtils::serializeData(mPosition));
    data.emplace_back("Roll", SerializationUtils::serializeData(mRoll));
    data.emplace_back("Pitch", SerializationUtils::serializeData(mPitch));
    data.emplace_back("Yaw", SerializationUtils::serializeData(mYaw));
    return data;
}

void ViewMatrixNode::deserializeDataExtra(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Position")
        SerializationUtils::deserializeData(stream, mPosition);
    else if (dataID == "Roll")
        SerializationUtils::deserializeData(stream, mRoll);
    else if (dataID == "Pitch")
        SerializationUtils::deserializeData(stream, mPitch);
    else if (dataID == "Yaw")
        SerializationUtils::deserializeData(stream, mYaw);
}

bool ViewMatrixNode::drawInputArea(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("Position");
    valueUpdated |= ImUtils::inputFloatN(&mPosition[0], 3, std::string(label).append("Position"));

    ImUtils::multiInputLabel("Roll", "Pitch", "Yaw");
    valueUpdated |= ImUtils::inputFloatN(&mRotations[0], 3, std::string(label).append("Rotation"), 0.0f, 360.0f);

    if (valueUpdated)
        mValue = generateViewMatrix();

    if (ImUtils::beginHeader("Data", generateNodeLabelID("DataHeader"), mShowData)) {
        ImGui::BeginDisabled();
        ImUtils::inputFloatNxN(&mValue[0][0], 4, generateNodeLabelID("MatrixData"));
        ImGui::EndDisabled();
        ImUtils::endHeader();
    }

    return valueUpdated;
}

ProjMatrixNode::ProjMatrixNode() : NumericNode<glm::mat4>("Projection") {
    mAspect = RenderConfig::getScreenAspect();
    mValue = generateProjMatrix();
}

std::vector<std::pair<std::string, std::string>> ProjMatrixNode::generateSerializedDataExtra() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Type", SerializationUtils::serializeData((unsigned int)mType));
    data.emplace_back("FoV", SerializationUtils::serializeData(mFoV));
    data.emplace_back("Aspect", SerializationUtils::serializeData(mAspect));
    data.emplace_back("ClipX", SerializationUtils::serializeData(mClipX));
    data.emplace_back("ClipY", SerializationUtils::serializeData(mClipY));
    data.emplace_back("ClipZ", SerializationUtils::serializeData(mClipZ));
    return data;
}

void ProjMatrixNode::deserializeDataExtra(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Type")
        SerializationUtils::deserializeData(stream, (unsigned int&)mType);
    else if (dataID == "FoV")
        SerializationUtils::deserializeData(stream, mFoV);
    else if (dataID == "Aspect")
        SerializationUtils::deserializeData(stream, mAspect);
    else if (dataID == "ClipX")
        SerializationUtils::deserializeData(stream, mClipX);
    else if (dataID == "ClipY")
        SerializationUtils::deserializeData(stream, mClipY);
    else if (dataID == "ClipZ")
        SerializationUtils::deserializeData(stream, mClipZ);
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

    if (valueUpdated)
        mValue = generateProjMatrix();

    if (ImUtils::beginHeader("Data", generateNodeLabelID("DataHeader"), mShowData)) {
        ImGui::BeginDisabled();
        ImUtils::inputFloatNxN(&mValue[0][0], 4, generateNodeLabelID("MatrixData"));
        ImGui::EndDisabled();
        ImUtils::endHeader();
    }

    return valueUpdated;
}

bool ProjMatrixNode::drawPerspective(const std::string& label) {
    bool valueUpdated = false;

    ImGui::Text("FoV");
    valueUpdated |= ImUtils::inputFloat(&mFoV, std::string(label).append("FoV"), 0.0f, 180.0f);

    ImGui::Text("Aspect Ratio");
    valueUpdated |= ImUtils::inputFloat(&mAspect, std::string(label).append("Aspect"), 0.0f, FLT_MAX);
    if (ImUtils::button("Screen Aspect", std::string(label).append("SetAspect"))) {
        valueUpdated = true;
        mAspect = RenderConfig::getScreenAspect();
    }

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
