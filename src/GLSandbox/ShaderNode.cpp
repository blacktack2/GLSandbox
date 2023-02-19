#include "ShaderNode.h"

#include <imgui/imgui.h>

#include <algorithm>
#include <filesystem>

inline std::vector<std::string> ShaderNode::sVertexFiles{};
inline std::vector<std::string> ShaderNode::sFragmentFiles{};
inline std::vector<std::string> ShaderNode::sTessContFiles{};
inline std::vector<std::string> ShaderNode::sTessEvalFiles{};
inline std::vector<std::string> ShaderNode::sGeometryFiles{};

inline const std::string ShaderNode::SHADER_DIR = "Shaders/";

inline const std::vector<std::string> ShaderNode::VALID_VERTEX    = {".glsl", ".vert", ".vs", ".glslv"};
inline const std::vector<std::string> ShaderNode::VALID_FRAGMENT  = {".glsl", ".frag", ".fs", ".glslf"};
inline const std::vector<std::string> ShaderNode::VALID_TESS_CONT = {".glsl", ".tesc", ".tcs", ".glsltc"};
inline const std::vector<std::string> ShaderNode::VALID_TESS_EVAL = {".glsl", ".tese", ".tes", ".glslte"};
inline const std::vector<std::string> ShaderNode::VALID_GEOMETRY  = {".glsl", ".geom", ".gs", ".glslg"};

ShaderNode::ShaderNode() : Node("Shader") {

}

void ShaderNode::findVertexFiles() {
    findFiles(sVertexFiles, VALID_VERTEX);
}

void ShaderNode::findFragmentFiles() {
    findFiles(sFragmentFiles, VALID_FRAGMENT);
}

void ShaderNode::findTessContFiles() {
    findFiles(sTessContFiles, VALID_TESS_CONT);
}

void ShaderNode::findTessEvalFiles() {
    findFiles(sTessEvalFiles, VALID_TESS_EVAL);
}

void ShaderNode::findGeometryFiles() {
    findFiles(sGeometryFiles, VALID_GEOMETRY);
}

void ShaderNode::drawContents() {
    drawShaderCombo("Vertex", vert, sVertexFiles);
    drawShaderCombo("Fragment", frag, sFragmentFiles);
    drawShaderCombo("Tess-Control", tesc, sTessContFiles);
    drawShaderCombo("Tess-Eval", tese, sTessEvalFiles);
    drawShaderCombo("Geometry", geom, sGeometryFiles);

    const std::string loadShaderLabel = std::string("Reload Shader##Button_LoadShader_Node").append(std::to_string(getID()));
    if (ImGui::Button(loadShaderLabel.c_str()))
        updateShader();

    drawShaderStatus();
}

void ShaderNode::runContents() {

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
            break;
        case Shader::ErrorState::VALID:
            text = "Shader loaded";
            break;
        case Shader::ErrorState::FILE_READ: case Shader::ErrorState::OGL_COMPILE: case Shader::ErrorState::OGL_LINK:
            text = mShader.getErrorMessage();
            break;
    }

    ImGui::Text("%s", text.c_str());
}

void ShaderNode::updateShader() {
    vertLoaded = (vert >= 0 && vert < sVertexFiles.size());
    fragLoaded = (frag >= 0 && frag < sFragmentFiles.size());
    tescLoaded = (tesc >= 0 && tesc < sTessContFiles.size());
    teseLoaded = (tese >= 0 && tese < sTessEvalFiles.size());
    geomLoaded = (geom >= 0 && geom < sGeometryFiles.size());

    std::string vertex   = vertLoaded ? sVertexFiles[vert]   : std::string();
    std::string fragment = fragLoaded ? sFragmentFiles[frag] : std::string();
    std::string tessCont = tescLoaded ? sTessContFiles[tesc] : std::string();
    std::string tessEval = teseLoaded ? sTessEvalFiles[tese] : std::string();
    std::string geometry = geomLoaded ? sGeometryFiles[geom] : std::string();

    if (vertex.empty() || fragment.empty() || tessCont.empty() != tessEval.empty()) {
        mShader = Shader();
        return;
    }
    mShader = Shader(
            SHADER_DIR + vertex,
            SHADER_DIR + fragment,
            tescLoaded ? SHADER_DIR + tessCont : "",
            teseLoaded ? SHADER_DIR + tessEval : "",
            geomLoaded ? SHADER_DIR + geometry : ""
    );
}

void ShaderNode::findFiles(std::vector<std::string>& files, const std::vector<std::string>& extensions) {
    files.clear();
    for (const auto& file : std::filesystem::directory_iterator(SHADER_DIR)) {
        if (!file.is_regular_file())
            continue;
        const std::string extension = file.path().extension().string();
        if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
            files.push_back(file.path().filename().string());
    }
}
