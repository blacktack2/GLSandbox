#include "ShaderNode.h"

#include "../Assets.h"

#include <algorithm>
#include <filesystem>

ShaderNode::ShaderNode() : Node("Shader") {
    addPort(mShaderOut);
}

bool ShaderNode::isValid() const {
    return mShader->getState() == Shader::ErrorState::VALID;
}

void ShaderNode::findVertexFiles() {
    findFiles(getVertexFiles(), getValidVertexFileExtensions());
}

void ShaderNode::findFragmentFiles() {
    findFiles(getFragmentFiles(), getValidFragmentFileExtensions());
}

void ShaderNode::findTessContFiles() {
    findFiles(getTessContFiles(), getValidTessContFileExtensions());
}

void ShaderNode::findTessEvalFiles() {
    findFiles(getTessEvalFiles(), getValidTessEvalFileExtensions());
}

void ShaderNode::findGeometryFiles() {
    findFiles(getGeometryFiles(), getValidGeometryFileExtensions());
}

std::vector<std::pair<std::string, std::string>> ShaderNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data;

    if (!mVertFile.empty())
        data.emplace_back("Vertex", mVertFile);
    if (!mFragFile.empty())
        data.emplace_back("Fragment", mFragFile);
    if (!mTescFile.empty())
        data.emplace_back("TessCont", mTescFile);
    if (!mTeseFile.empty())
        data.emplace_back("TessEval", mTeseFile);
    if (!mGeomFile.empty())
        data.emplace_back("Geometry", mGeomFile);

    return data;
}

void ShaderNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Vertex") {
        stream >> mVertFile;
    } else if (dataID == "Fragment") {
        stream >> mFragFile;
    } else if (dataID == "TessCont") {
        stream >> mTescFile;
    } else if (dataID == "TessEval") {
        stream >> mTeseFile;
    } else if (dataID == "Geometry") {
        stream >> mGeomFile;
    }
}
void ShaderNode::onDeserialize() {
    uploadShader();
}

void ShaderNode::drawContents() {
    ImUtils::fileChooseDialog(mVertFile, generateNodeLabelID("VertexFile"));
    ImUtils::fileChooseDialog(mFragFile, generateNodeLabelID("FragmentFile"));
    ImUtils::fileChooseDialog(mTescFile, generateNodeLabelID("Tess-ContFile"));
    ImUtils::fileChooseDialog(mTeseFile, generateNodeLabelID("Tess-EvalFile"));
    ImUtils::fileChooseDialog(mGeomFile, generateNodeLabelID("GeometryFile"));

    if (ImUtils::button("Upload", generateNodeLabelID("Upload"))) {
        uploadShader();
        mShaderOut.valueUpdated();
    }

    drawShaderStatus();
}

void ShaderNode::drawShaderStatus() {
    std::string text;
    ImVec4 colour;

    switch (mShader->getState()) {
        case Shader::ErrorState::INVALID:
            if (mVertFile.empty())
                text = "No vertex shader selected";
            else if (mFragFile.empty())
                text = "No fragment shader selected";
            else if (!mTescFile.empty() && mTeseFile.empty())
                text = "Tesselation Control selected, but no Tesselation Evaluation";
            else if (mTescFile.empty() && !mTeseFile.empty())
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
    if (mVertFile.empty() || mFragFile.empty() || (mTescFile.empty() != mTeseFile.empty())) {
        mShader = std::make_unique<Shader>();
        return;
    }
    mShader = std::make_unique<Shader>(mVertFile, mFragFile, mTescFile, mTeseFile, mGeomFile);
}

void ShaderNode::findFiles(std::vector<std::string>& files, const std::vector<std::string>& extensions) {
    files.clear();
    for (const auto& file : std::filesystem::directory_iterator(gSHADER_ASSET_DIR)) {
        if (!file.is_regular_file())
            continue;

        const std::string extension = file.path().extension().string();
        if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
            files.push_back(file.path().filename().string());
    }
}

