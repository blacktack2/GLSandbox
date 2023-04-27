#include "ShaderNode.h"

#include "../../Utils/SerializationUtils.h"

#include <algorithm>

ShaderNode::ShaderNode() : Node("Shader") {
    addPort(mShaderOut);
}

bool ShaderNode::isValid() const {
    return mShader->getState() == Shader::ErrorState::VALID;
}

std::vector<std::pair<std::string, std::string>> ShaderNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data;

    if (!mVertData.filepath.empty())
        data.emplace_back("Vertex", mVertData.filepath.string());
    if (!mFragData.filepath.empty())
        data.emplace_back("Fragment", mFragData.filepath.string());
    if (!mTescData.filepath.empty())
        data.emplace_back("TessCont", mTescData.filepath.string());
    if (!mTeseData.filepath.empty())
        data.emplace_back("TessEval", mTeseData.filepath.string());
    if (!mGeomData.filepath.empty())
        data.emplace_back("Geometry", mGeomData.filepath.string());

    return data;
}

void ShaderNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Vertex") {
        mVertData.filepath = SerializationUtils::readLine(stream);
        SerializationUtils::readFile(mVertData.filepath, mVertData.code);
    } else if (dataID == "Fragment") {
        mFragData.filepath = SerializationUtils::readLine(stream);
        SerializationUtils::readFile(mFragData.filepath, mFragData.code);
    } else if (dataID == "TessCont") {
        mTescData.filepath = SerializationUtils::readLine(stream);
        SerializationUtils::readFile(mTescData.filepath, mTescData.code);
    } else if (dataID == "TessEval") {
        mTeseData.filepath = SerializationUtils::readLine(stream);
        SerializationUtils::readFile(mTeseData.filepath, mTeseData.code);
    } else if (dataID == "Geometry") {
        mGeomData.filepath = SerializationUtils::readLine(stream);
        SerializationUtils::readFile(mGeomData.filepath, mGeomData.code);
    }
}
void ShaderNode::onDeserialize() {
    uploadShader();
}

void ShaderNode::drawContents() {
    drawShaderInput(mVertData);
    drawShaderInput(mFragData);
    drawShaderInput(mTescData);
    drawShaderInput(mTeseData);
    drawShaderInput(mGeomData);

    ImGui::BeginDisabled(isLocked());

    if (ImUtils::button("Upload", generateNodeLabelID("Upload"))) {
        uploadShader();
        mShaderOut.valueUpdated();
    }

    ImGui::EndDisabled();

    drawShaderStatus();
}

void ShaderNode::drawShaderInput(ShaderPassData& data) {
    ImGui::BeginDisabled(isLocked());

    bool fileChosen = ImUtils::fileChooseDialog(
        data.filepath, getShaderAssetDirectory(), generateNodeLabelID(data.displayName, "File"),
        getValidVertexShaderFileExtensions()
    );
    if (fileChosen) {
        if (SerializationUtils::readFile(data.filepath, data.code))
            data.state = ShaderFileState::ReadSuccess;
        else
            data.state = ShaderFileState::ReadFailure;
    }

    ImGui::EndDisabled();

    if (data.code.empty()) {
        if (data.filepath.empty())
            drawMessage("File not selected", ImVec4(1, 1, 0, 1));
        else
            drawMessage("Error reading file", ImVec4(1, 0, 0, 1));
    } else {
        ImUtils::dataPanelButton(
            data.displayName, generateNodeLabelID(data.displayName, "Panel"), *this,
            [this, &data]() {
                if (ImUtils::button("Remove Pass", generateNodeLabelID(data.displayName, "Remove"))) {
                    data.filepath = "";
                    data.code = "";
                    data.state = ShaderFileState::Unloaded;
                    ImUtils::softUnsetDataPanel(*this);
                    return;
                }

                ImGui::TextUnformatted(data.filepath.filename().string().c_str());

                ImGui::BeginDisabled(isLocked());

                ImUtils::inputTextAreaFile(data.code, generateNodeLabelID(data.displayName, "Code"));

                ImGui::EndDisabled();
            }
        );
    }
}

void ShaderNode::drawShaderStatus() {
    std::string text;
    ImVec4 colour;

    switch (mShader->getState()) {
        case Shader::ErrorState::INVALID:
            if (mVertData.filepath.empty())
                text = "No vertex shader selected";
            else if (mFragData.filepath.empty())
                text = "No fragment shader selected";
            else if (!mTescData.filepath.empty() && mTeseData.filepath.empty())
                text = "Tesselation Control selected, but no Tesselation Evaluation";
            else if (mTescData.filepath.empty() && !mTeseData.filepath.empty())
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
    if (mVertData.filepath.empty() || mFragData.filepath.empty() || (mTescData.filepath.empty() != mTeseData.filepath.empty())) {
        mShader = std::make_unique<Shader>();
        return;
    }
    mShader = std::make_unique<Shader>(
        mVertData.code, mFragData.code, mTescData.code, mTeseData.code, mGeomData.code
    );
}
