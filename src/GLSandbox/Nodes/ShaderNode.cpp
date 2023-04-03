#include "ShaderNode.h"

#include "../Assets.h"

#include <imgui.h>

#include <algorithm>
#include <filesystem>

ShaderNode::ShaderNode() : Node("Shader") {
    addPort(mShaderOutPort);
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

void ShaderNode::serializeContents(std::ofstream& streamOut) const {

}

void ShaderNode::deserializeContents(std::ifstream& streamIn) {

}

void ShaderNode::drawContents() {
    drawShaderChooseButton("Vertex",    mVertFile, findVertexFiles,   getVertexFiles);
    drawShaderChooseButton("Fragment",  mFragFile, findFragmentFiles, getFragmentFiles);
    drawShaderChooseButton("Tess-Cont", mTescFile, findTessContFiles, getTessContFiles);
    drawShaderChooseButton("Tess-Eval", mTeseFile, findTessEvalFiles, getTessEvalFiles);
    drawShaderChooseButton("Geometry",  mGeomFile, findGeometryFiles, getGeometryFiles);

    const std::string loadShaderLabel = std::string("Reload Shader##Button_LoadShader_Node").append(std::to_string(getID()));
    if (ImGui::Button(loadShaderLabel.c_str()))
        updateShader();

    drawShaderStatus();
}

void ShaderNode::drawShaderChooseButton(const std::string& shaderType, std::string& value,
                                        const update_files_callback& updateFiles, const get_files_callback& getFiles) {
    ImGui::Text("%s: %s", shaderType.c_str(), value.empty() ? "<not-selected>" : value.c_str());

    const std::string POPUP_ID = std::string("Choose").append(shaderType).append("Popup_Node")
            .append(std::to_string(getID()));
    if (ImGui::Button(std::string("Choose ").append(shaderType).c_str())) {
        ImGui::OpenPopup(POPUP_ID.c_str());
        updateFiles();
    }
    drawShaderChoosePopup(POPUP_ID, getFiles(), value);
}

void ShaderNode::drawShaderChoosePopup(const std::string& popupLabel, const std::vector<std::string>& files, std::string& value) {
    if (ImGui::BeginPopup(popupLabel.c_str())) {
        if (files.empty()) {
            ImGui::Text("No files found");
        } else {
            for (const std::string& file : files) {
                const std::string FILE_SELECT_LABEL = std::string(file).append("##FileSelect_Node").append(
                        std::to_string(getID()));
                if (!ImGui::Selectable(FILE_SELECT_LABEL.c_str()))
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

void ShaderNode::updateShader() {
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
