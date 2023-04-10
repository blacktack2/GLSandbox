#include "ShaderNode.h"

#include "../Assets.h"

#include <imgui.h>

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
    drawShaderChooseButton("Vertex",    mVertFile, findVertexFiles,   getVertexFiles);
    drawShaderChooseButton("Fragment",  mFragFile, findFragmentFiles, getFragmentFiles);
    drawShaderChooseButton("Tess-Cont", mTescFile, findTessContFiles, getTessContFiles);
    drawShaderChooseButton("Tess-Eval", mTeseFile, findTessEvalFiles, getTessEvalFiles);
    drawShaderChooseButton("Geometry",  mGeomFile, findGeometryFiles, getGeometryFiles);

    const std::string loadShaderButtonLabel = generateNodeLabel("Reload Shader");
    if (ImGui::Button(loadShaderButtonLabel.c_str())) {
        uploadShader();
        mShaderOut.valueUpdated();
    }

    drawShaderStatus();
}

void ShaderNode::drawShaderChooseButton(const std::string& shaderType, std::string& value,
                                        const update_files_callback& updateFiles, const get_files_callback& getFiles) {
    ImGui::Text("%s: %s", shaderType.c_str(), value.empty() ? "<not-selected>" : value.c_str());

    const std::string choseShaderPopupID = generateNodePopupID("ChooseShader", shaderType);
    const std::string chooseShaderButtonLabel = generateNodeLabel(std::string("Choose ").append(shaderType));
    if (ImGui::Button(chooseShaderButtonLabel.c_str())) {
        ImGui::OpenPopup(choseShaderPopupID.c_str());
        updateFiles();
    }
    drawShaderChoosePopup(choseShaderPopupID, getFiles(), value);
}

void ShaderNode::drawShaderChoosePopup(const std::string& popupID, const std::vector<std::string>& files, std::string& value) {
    if (ImGui::BeginPopup(popupID.c_str())) {
        if (files.empty()) {
            ImGui::Text("No files found");
        } else {
            for (const std::string& file : files) {
                const std::string fileSelectButtonLabel = generateNodeLabel(file, "FileSelect");
                if (!ImGui::Selectable(fileSelectButtonLabel.c_str()))
                    continue;
                value = file;
                break;
            }
        }

        ImGui::EndPopup();
    }
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

    ImGui::TextColored(colour, "%s", text.c_str());
}

void ShaderNode::uploadShader() {
    if (mVertFile.empty() || mFragFile.empty() || (mTescFile.empty() != mTeseFile.empty())) {
        mShader = std::make_unique<Shader>();
        return;
    }
    mShader = std::make_unique<Shader>(
        gSHADER_ASSET_DIR + mVertFile,
        gSHADER_ASSET_DIR + mFragFile,
        mTescFile.empty() ? "" : gSHADER_ASSET_DIR + mTescFile,
        mTeseFile.empty() ? "" : gSHADER_ASSET_DIR + mTeseFile,
        mGeomFile.empty() ? "" : gSHADER_ASSET_DIR + mGeomFile
    );
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

