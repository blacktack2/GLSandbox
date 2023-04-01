#include "ShaderNode.h"

#include <imgui.h>

#include <algorithm>
#include <filesystem>

ShaderNode::ShaderNode() : Node("Shader") {
    addPort(mShaderOutPort);
}

bool ShaderNode::isValid() const {
    return mShader.getState() == Shader::ErrorState::VALID;
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

void ShaderNode::drawContents() {
    drawShaderCombo("Vertex", vert, getVertexFiles());
    drawShaderCombo("Fragment", frag, getFragmentFiles());
    drawShaderCombo("Tess-Control", tesc, getTessContFiles());
    drawShaderCombo("Tess-Eval", tese, getTessEvalFiles());
    drawShaderCombo("Geometry", geom, getGeometryFiles());

    const std::string loadShaderLabel = std::string("Reload Shader##Button_LoadShader_Node").append(std::to_string(getID()));
    if (ImGui::Button(loadShaderLabel.c_str()))
        updateShader();

    drawShaderStatus();
}

void ShaderNode::drawShaderCombo(const std::string& label, int& index, const std::vector<std::string>& files) {
    static const float COMBO_OFFSET = 90.0f;
    static const float COMBO_WIDTH = 90.0f;

    ImGui::Text("%s", label.c_str());
    ImGui::SameLine(COMBO_OFFSET);
    const std::string comboLabel = std::string("##Combo_").append(label).append("_Node").append(std::to_string(getID()));
    ImGui::SetNextItemWidth(COMBO_WIDTH);
    if (ImGui::BeginCombo(comboLabel.c_str(), index < 0 ? "" : files[index].c_str())) {
        for (size_t n = 0; n < files.size(); n++) {
            const bool isSelected = index == n;
            if (ImGui::Selectable(files[n].c_str(), isSelected))
                index = n;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void ShaderNode::drawShaderStatus() {
    std::string text;
    ImVec4 colour;

    switch (mShader.getState()) {
        case Shader::ErrorState::INVALID:
            if (!vertLoaded)
                text = "No vertex shader selected";
            else if (!fragLoaded)
                text = "No fragment shader selected";
            else if (tescLoaded && !teseLoaded)
                text = "Tesselation Control selected, but no Tesselation Evaluation";
            else if (!tescLoaded && teseLoaded)
                text = "Tesselation Evaluation selected, but no Tesselation Control";
            else
                text = mShader.getErrorMessage();
            colour = ImVec4(1, 0, 0, 1);
            break;
        case Shader::ErrorState::VALID:
            text = "Shader loaded";
            colour = ImVec4(0, 1, 0, 1);
            break;
        case Shader::ErrorState::FILE_READ: case Shader::ErrorState::OGL_COMPILE: case Shader::ErrorState::OGL_LINK:
            text = mShader.getErrorMessage();
            colour = ImVec4(1, 0, 0, 1);
            break;
    }

    ImGui::TextColored(colour, "%s", text.c_str());
}

void ShaderNode::updateShader() {
    vertLoaded = (vert >= 0 && vert < getVertexFiles().size());
    fragLoaded = (frag >= 0 && frag < getFragmentFiles().size());
    tescLoaded = (tesc >= 0 && tesc < getTessContFiles().size());
    teseLoaded = (tese >= 0 && tese < getTessEvalFiles().size());
    geomLoaded = (geom >= 0 && geom < getGeometryFiles().size());

    std::string vertex   = vertLoaded ? getVertexFiles()[vert]   : std::string();
    std::string fragment = fragLoaded ? getFragmentFiles()[frag] : std::string();
    std::string tessCont = tescLoaded ? getTessContFiles()[tesc] : std::string();
    std::string tessEval = teseLoaded ? getTessEvalFiles()[tese] : std::string();
    std::string geometry = geomLoaded ? getGeometryFiles()[geom] : std::string();

    if (vertex.empty() || fragment.empty() || tessCont.empty() != tessEval.empty()) {
        mShader = Shader();
        return;
    }
    mShader = Shader(
        getShaderDir() + vertex,
        getShaderDir() + fragment,
        tescLoaded ? getShaderDir() + tessCont : "",
        teseLoaded ? getShaderDir() + tessEval : "",
        geomLoaded ? getShaderDir() + geometry : ""
    );
}

void ShaderNode::findFiles(std::vector<std::string>& files, const std::vector<std::string>& extensions) {
    files.clear();
    for (const auto& file : std::filesystem::directory_iterator(getShaderDir())) {
        if (!file.is_regular_file())
            continue;
        const std::string extension = file.path().extension().string();
        if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
            files.push_back(file.path().filename().string());
    }
}
