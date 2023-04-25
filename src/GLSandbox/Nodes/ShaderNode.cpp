#include "ShaderNode.h"

#include "../../Utils/SerializationUtils.h"

#include "../Assets.h"

#include <algorithm>

ShaderNode::ShaderNode() : Node("Shader") {
    addPort(mShaderOut);
}

bool ShaderNode::isValid() const {
    return mShader->getState() == Shader::ErrorState::VALID;
}

std::vector<std::pair<std::string, std::string>> ShaderNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data;

    if (!mVertFilepath.empty())
        data.emplace_back("Vertex", mVertFilepath.string());
    if (!mFragFilepath.empty())
        data.emplace_back("Fragment", mFragFilepath.string());
    if (!mTescFilepath.empty())
        data.emplace_back("TessCont", mTescFilepath.string());
    if (!mTeseFilepath.empty())
        data.emplace_back("TessEval", mTeseFilepath.string());
    if (!mGeomFilepath.empty())
        data.emplace_back("Geometry", mGeomFilepath.string());

    return data;
}

void ShaderNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Vertex")
        mVertFilepath = SerializationUtils::readLine(stream);
    else if (dataID == "Fragment")
        mFragFilepath = SerializationUtils::readLine(stream);
    else if (dataID == "TessCont")
        mTescFilepath = SerializationUtils::readLine(stream);
    else if (dataID == "TessEval")
        mTeseFilepath = SerializationUtils::readLine(stream);
    else if (dataID == "Geometry")
        mGeomFilepath = SerializationUtils::readLine(stream);
}
void ShaderNode::onDeserialize() {
    uploadShader();
}

void ShaderNode::drawContents() {
    ImGui::BeginDisabled(isLocked());

    ImUtils::fileChooseDialog(mVertFilepath, getShaderAssetDirectory(), generateNodeLabelID("VertexFile"),
                              getValidVertexShaderFileExtensions());
    ImUtils::fileChooseDialog(mFragFilepath, getShaderAssetDirectory(), generateNodeLabelID("FragmentFile"),
                              getValidFragmentShaderFileExtensions());
    ImUtils::fileChooseDialog(mTescFilepath, getShaderAssetDirectory(), generateNodeLabelID("Tess-ContFile"),
                              getValidTessContShaderFileExtensions());
    ImUtils::fileChooseDialog(mTeseFilepath, getShaderAssetDirectory(), generateNodeLabelID("Tess-EvalFile"),
                              getValidTessEvalShaderFileExtensions());
    ImUtils::fileChooseDialog(mGeomFilepath, getShaderAssetDirectory(), generateNodeLabelID("GeometryFile"),
                              getValidGeometryShaderFileExtensions());

    if (ImUtils::button("Upload", generateNodeLabelID("Upload"))) {
        uploadShader();
        mShaderOut.valueUpdated();
    }

    ImGui::EndDisabled();

    drawShaderStatus();
}

void ShaderNode::drawShaderStatus() {
    std::string text;
    ImVec4 colour;

    switch (mShader->getState()) {
        case Shader::ErrorState::INVALID:
            if (mVertFilepath.empty())
                text = "No vertex shader selected";
            else if (mFragFilepath.empty())
                text = "No fragment shader selected";
            else if (!mTescFilepath.empty() && mTeseFilepath.empty())
                text = "Tesselation Control selected, but no Tesselation Evaluation";
            else if (mTescFilepath.empty() && !mTeseFilepath.empty())
                text = "Tesselation Evaluation selected, but no Tesselation Control";
            else
                text = mShader->getErrorMessage();
            colour = ImVec4(1, 0, 0, 1);
            break;
        case Shader::ErrorState::VALID:
            text = "Shader loaded";
            colour = ImVec4(0, 1, 0, 1);
            break;
        case Shader::ErrorState::FILE_READ: case Shader::ErrorState::OGL_COMPILE: case Shader::ErrorState::OGL_LINK:
            text = mShader->getErrorMessage();
            colour = ImVec4(1, 0, 0, 1);
            break;
    }

    drawMessage(text, colour);
}

void ShaderNode::uploadShader() {
    if (mVertFilepath.empty() || mFragFilepath.empty() || (mTescFilepath.empty() != mTeseFilepath.empty())) {
        mShader = std::make_unique<Shader>();
        return;
    }
    mShader = std::make_unique<Shader>(mVertFilepath, mFragFilepath, mTescFilepath, mTeseFilepath, mGeomFilepath);
}
