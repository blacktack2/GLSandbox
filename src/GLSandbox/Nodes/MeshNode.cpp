#include "MeshNode.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <filesystem>
#include <regex>
#include <unordered_set>

MeshNode::MeshNode() : Node("Mesh") {
    mMesh = std::make_unique<Mesh>();

    addPort(mMeshOut);

    mFilename = generateFilename();
}

std::vector<std::pair<std::string, std::string>> MeshNode::generateSerializedData() const {
    std::string filename = mFilename;
    if (filename.empty())
        filename = generateFilename();

    writeToFile();

    filename.append(getFileExtension(mFileExtension));

    return {
        {"File", filename},
    };
}

void MeshNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "File") {
        std::string filename;
        stream >> filename;
        std::size_t index = filename.find_last_of('.');
        mFilename = filename.substr(0, index);
        mFileExtension = parseFileExtension(filename.substr(index).c_str());
        loadFromFile();
    }
}

void MeshNode::onDeserialize() {
    uploadMesh();
}

void MeshNode::drawContents() {
    drawGlobalParameters();


    const std::string FILENAME_LABEL = std::string("File:##").append(getNodeID());
    ImGui::InputText(FILENAME_LABEL.c_str(), &mFilename);
    ImGui::SameLine();
    const std::string LOAD_FILE_BUTTON = std::string("Load##").append(getNodeID());
    if (ImGui::Button(LOAD_FILE_BUTTON.c_str()))
        loadFromFile();

    const std::string SHOW_ATTR_BUTTON_LABEL = std::string("Show Attributes##").append(getNodeID());
    if (ImGui::Button(SHOW_ATTR_BUTTON_LABEL.c_str()))
        mShowAttributes = !mShowAttributes;
    if (mShowAttributes) {
        drawAttributes();
        drawAddAttributePopup();
    }
    drawUploadButton();

    drawMeshStatus();
}

void MeshNode::loadFromFile() {
    const std::string fileExtension = getFileExtension(mFileExtension);
    const std::string fullPath = std::string(gMESH_ASSET_DIR).append(mFilename).append(fileExtension);

    clearMesh();

    std::ifstream stream(fullPath);
    if (stream)
        loadFromStream(stream, fileExtension);
}

void MeshNode::loadFromStream(std::ifstream& stream, const std::string& extension) {
    if (extension == ".obj")
        loadFromStreamOBJ(stream);
    else if (extension == ".msh")
        loadFromStreamMSH(stream);
}

void MeshNode::loadFromStreamOBJ(std::ifstream& stream) {
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    while (!stream.eof()) {
        std::string type;
        stream >> type;
        if (type == "v") {
            glm::vec3 vertex;
            stream >> vertex.x;
            stream >> vertex.y;
            stream >> vertex.z;
            positions.emplace_back(vertex);
        } else if (type == "vt") {
            glm::vec2 uv;
            stream >> uv.x;
            stream >> uv.y;
            uvs.emplace_back(uv);
        } else if (type == "vn") {
            glm::vec3 normal;
            stream >> normal.x;
            stream >> normal.y;
            stream >> normal.z;
            normals.emplace_back(normal);
        } else if (type == "f") {
            glm::vec3 v1, v2, v3;
            stream >> v1.x;
            stream >> v1.y;
            stream >> v1.z;
            indices.push_back(v1.x);
            indices.push_back(v1.y);
            indices.push_back(v1.z);
            stream >> v2.x;
            stream >> v2.y;
            stream >> v2.z;
            indices.push_back(v2.x);
            indices.push_back(v2.y);
            indices.push_back(v2.z);
            stream >> v3.x;
            stream >> v3.y;
            stream >> v3.z;
            indices.push_back(v3.x);
            indices.push_back(v3.y);
            indices.push_back(v3.z);
        } else {
            SerializationUtils::skipToNextLine(stream);
            continue;
        }
    }

    mNumVertices = positions.size();
    mNumIndices = indices.size();

    uvs.resize(mNumVertices);
    normals.resize(mNumVertices);

    mType = Mesh::Type::Triangles;

    mIndices = indices;

    Attribute posAttr;
    posAttr.data = positions;
    posAttr.name = "position";

    Attribute uvAttr;
    uvAttr.data = uvs;
    uvAttr.name = "uv";

    Attribute normAttr;
    normAttr.data = normals;
    normAttr.name = "normal";
}

void MeshNode::loadFromStreamMSH(std::ifstream& stream) {
    SerializationUtils::skipToNextLine(stream);
    stream >> mNumVertices;
    stream >> mNumIndices;
    unsigned int type;
    stream >> type;
    mType = (Mesh::Type)type;

    while (!stream.eof()) {
        std::string name;
        stream >> name;

        if (name.empty() || name[0] == '#') {
            SerializationUtils::skipToNextLine(stream);
            continue;
        }

        unsigned int length;
        char dataType;
        stream >> length;
        stream >> dataType;

        Attribute& attribute = mAttributes.emplace_back();
        attribute.data = createAttributeDataset(length, dataType);
        attribute.name = name;
        std::visit([this](auto& arg) { arg.resize(mNumVertices); }, attribute.data);

        std::visit(VisitOverload{
            [&stream, this](std::vector<int>& data) {
                for (unsigned int i = 0; i < mNumVertices; i++)
                    stream >> data[i];
            },
            [&stream, this](std::vector<float>& data) {
                for (unsigned int i = 0; i < mNumVertices; i++)
                    stream >> data[i];
            },
            [&stream, this](auto& data) {
                for (unsigned int i = 0; i < mNumVertices; i++)
                    for (unsigned int j = 0; j < data[i].length(); j++)
                        stream >> data[i][j];
            },
        }, attribute.data);
    }
}

void MeshNode::writeToFile() const {
    if (mFileExtension == gMESH_DEFAULT_EXTENSION) {
        const std::string fileExtension = getFileExtension(gMESH_DEFAULT_EXTENSION);
        const std::string fullPath = std::string(gMESH_ASSET_DIR).append(mFilename).append(fileExtension);
        std::ofstream stream(fullPath);
        writeToStreamMSH(stream);
    }
}

void MeshNode::writeToStreamMSH(std::ofstream& stream) const {
    stream << "# GLSandbox MSH File: " << mFilename << "\n";

    stream << mNumVertices << " " << mNumIndices << " " << (unsigned int)mType << "\n";

    for (const auto& attribute : mAttributes) {
        stream << attribute.name << " ";

        std::visit(VisitOverload{
            [&stream](const std::vector<int>&        arg) { stream << 1 << " " << 'i'; },
            [&stream](const std::vector<glm::ivec2>& arg) { stream << 2 << " " << 'i'; },
            [&stream](const std::vector<glm::ivec3>& arg) { stream << 3 << " " << 'i'; },
            [&stream](const std::vector<glm::ivec4>& arg) { stream << 4 << " " << 'i'; },

            [&stream](const std::vector<float>&      arg) { stream << 1 << " " << 'f'; },
            [&stream](const std::vector<glm::vec2>&  arg) { stream << 2 << " " << 'f'; },
            [&stream](const std::vector<glm::vec3>&  arg) { stream << 3 << " " << 'f'; },
            [&stream](const std::vector<glm::vec4>&  arg) { stream << 4 << " " << 'f'; },

            [&stream](const auto&                  arg) { stream << 1 << " " << "i"; },
        }, attribute.data);
        stream << "\n";

        std::visit(VisitOverload{
            [&stream, this](const std::vector<int>& data) {
                for (unsigned int i = 0; i < mNumVertices; i++)
                    stream << data[i] << "\n";
            },
            [&stream, this](const std::vector<float>& data) {
                for (unsigned int i = 0; i < mNumVertices; i++)
                    stream << data[i] << "\n";
            },
            [&stream, this](const auto& data) {
                for (unsigned int i = 0; i < mNumVertices; i++) {
                    stream << data[i][0];
                    for (unsigned int j = 1; j < data[i].length(); j++)
                        stream << " " << data[i][j];
                    stream << "\n";
                }
            },
        }, attribute.data);
    }
    for (unsigned int index : mIndices) {
        stream << "i " << index << "\n";
    }
}

std::string MeshNode::generateFilename() {
    static const std::regex cDEFAULT_FILE_REGEX("^Mesh(0-9)+$");

    std::unordered_set<unsigned int> paths;
    for (auto const& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(gMESH_ASSET_DIR))) {
        if (!std::filesystem::is_regular_file(entry) ||
            parseFileExtension(entry.path().extension().c_str()) == MeshFileExtension::Undefined)
            continue;
        std::smatch matches;
        std::string filename = entry.path().filename().string();
        if (std::regex_search(filename, matches, cDEFAULT_FILE_REGEX))
            paths.emplace(std::stoi(matches[1]));
    }
    unsigned int counter = 0;
    while (paths.contains(++counter)) {}

    return std::string("Mesh").append(std::to_string(counter));
}

void MeshNode::uploadMesh() {
    mMesh->hardClean();
    mMesh->setType(mType);
    mMesh->setNumVertices(mNumVertices);
    for (const auto& attr : mAttributes) {
        std::visit(VisitOverload{
            [&attr, this](const std::vector<int>& data) {
                using type = std::decay_t<decltype(data)>;
                mMesh->addAttribute(data.data(), 1, sizeof(int), attr.name);
            },
            [&attr, this](const std::vector<float>& data) {
                using type = std::decay_t<decltype(data)>;
                mMesh->addAttribute(data.data(), 1, sizeof(float), attr.name);
            },
            [&attr, this](const auto& data) {
                using type = std::decay_t<decltype(data)>;
                mMesh->addAttribute(data.data(), type::value_type::length(), sizeof(typename type::value_type), attr.name);
            },
        }, attr.data);
    }

    mMesh->bufferData();
}

void MeshNode::drawGlobalParameters() {
    const std::string NUM_VERTICES_LABEL = std::string("Num Vertices##NumVertices_").append(getNodeID());
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::InputInt(NUM_VERTICES_LABEL.c_str(), reinterpret_cast<int*>(&mNumVertices))) {
        mNumVertices = std::max(mNumVertices, 1u);
        resizeAttributes();
    }
    const std::string TYPE_LABEL = std::string("Direction##Type_").append(getNodeID());
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::BeginCombo(TYPE_LABEL.c_str(), mTypes[(size_t)mType].c_str())) {
        for (size_t i = 0; i < mTypes.size(); i++) {
            const bool isSelected = (size_t)mType == i;
            if (ImGui::Selectable(mTypes[i].c_str(), isSelected))
                mType = (Mesh::Type)i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void MeshNode::drawAttributes() {
    for (auto& attribute : mAttributes)
        drawAttribute(attribute);

    mAttributes.erase(
        std::remove_if(mAttributes.begin(), mAttributes.end(),
            [](const auto& attr) { return attr.isMarkedDelete; }
        ), mAttributes.end()
    );
}

void MeshNode::drawAttribute(Attribute& attribute) {
    const std::string ATTRIBUTE_SHOW_LABEL = std::string(attribute.name).append(" (")
            .append(getDataTypeName(attribute.data)).append(")")
            .append("##AttributeHeader_").append(std::to_string(attribute.id)).append(getAttributeID(attribute));
    const float BUTTON_SIZE = 200.0f;
    if (ImGui::Button(ATTRIBUTE_SHOW_LABEL.c_str(), ImVec2(BUTTON_SIZE, 0)))
        attribute.isShown = !attribute.isShown;
    ImGui::SameLine();
    const std::string ATTRIBUTE_REMOVE_LABEL = std::string("Remove")
            .append("##AttributeHeader_").append(std::to_string(attribute.id)).append(getAttributeID(attribute));
    if (ImGui::Button(ATTRIBUTE_REMOVE_LABEL.c_str(), ImVec2(BUTTON_SIZE, 0))) {
        attribute.isMarkedDelete = true;
        return;
    }
    if (!attribute.isShown)
        return;

    ImGui::Indent(16.0f);
    const std::string ATTRIBUTE_NAME_LABEL = std::string("name##AttributeName_")
            .append(std::to_string(attribute.id)).append(getAttributeID(attribute));
    ImGui::InputText(ATTRIBUTE_NAME_LABEL.c_str(), &attribute.name);

    for (unsigned int i = 0; i < mNumVertices; i++) {
        const std::string VALUE_LABEL = std::to_string(i).append("##AttributeValue_")
                .append(std::to_string(attribute.id)).append(getAttributeID(attribute));
        std::visit(VisitOverload{
            [VALUE_LABEL, i](std::vector<int>& data       ) { ImGui::InputInt   (VALUE_LABEL.c_str(), &data[i]   ); },
            [VALUE_LABEL, i](std::vector<glm::ivec2>& data) { ImGui::InputInt2  (VALUE_LABEL.c_str(), &data[i][0]); },
            [VALUE_LABEL, i](std::vector<glm::ivec3>& data) { ImGui::InputInt3  (VALUE_LABEL.c_str(), &data[i][0]); },
            [VALUE_LABEL, i](std::vector<glm::ivec4>& data) { ImGui::InputInt4  (VALUE_LABEL.c_str(), &data[i][0]); },
            [VALUE_LABEL, i](std::vector<float>& data     ) { ImGui::InputFloat (VALUE_LABEL.c_str(), &data[i]   ); },
            [VALUE_LABEL, i](std::vector<glm::vec2>& data ) { ImGui::InputFloat2(VALUE_LABEL.c_str(), &data[i][0]); },
            [VALUE_LABEL, i](std::vector<glm::vec3>& data ) { ImGui::InputFloat3(VALUE_LABEL.c_str(), &data[i][0]); },
            [VALUE_LABEL, i](std::vector<glm::vec4>& data ) { ImGui::InputFloat4(VALUE_LABEL.c_str(), &data[i][0]); },
            [](auto arg) { ImGui::Text("Undefined"); },
        }, attribute.data);
    }
    ImGui::Unindent(0.0f);
}

void MeshNode::drawAddAttributePopup() {
    const std::string ADD_ATTRIBUTE_LABEL = std::string("Add Attribute##AddAttributeButton_").append(getNodeID());
    const std::string ADD_ATTRIBUTE_POPUP_ID = std::string("AddAttributePopup").append(getNodeID());
    if (ImGui::Button(ADD_ATTRIBUTE_LABEL.c_str()))
        ImGui::OpenPopup(ADD_ATTRIBUTE_POPUP_ID.c_str());

    if (ImGui::BeginPopup(ADD_ATTRIBUTE_POPUP_ID.c_str())) {
        drawAttributeSelection();

        ImGui::EndPopup();
    }
}

void MeshNode::drawAttributeSelection() {
    for (unsigned int i = 0; i < (unsigned int)Mesh::AttributeType::Max; i++) {
        Mesh::AttributeType type = (Mesh::AttributeType)i;
        const std::string ATTRIBUTE_ID = std::string(getDataTypeName(type)).append("##Selectable_").append(getNodeID());
        if (ImGui::Selectable(ATTRIBUTE_ID.c_str())) {
            Attribute& attribute = mAttributes.emplace_back();
            std::visit([this](auto& data) { data.resize(mNumVertices); }, attribute.data);
        }
    }
}

void MeshNode::drawUploadButton() {
    const std::string UPLOAD_BUTTON_LABEL = std::string("Upload##UploadButton_").append(getNodeID());
    if (ImGui::Button(UPLOAD_BUTTON_LABEL.c_str()))
        uploadMesh();
}

void MeshNode::drawMeshStatus() {
    std::string text;
    ImVec4 colour;
    switch (mMesh->getState()) {
        default:
        case Mesh::ErrorState::INVALID:
            text = "Mesh not uploaded";
            colour = ImVec4(1, 0, 0, 1);
            break;
        case Mesh::ErrorState::VALID:
            text = "Mesh loaded";
            colour = ImVec4(0, 1, 0, 1);
            break;
    }

    ImGui::TextColored(colour, "%s", text.c_str());
}

std::string MeshNode::getNodeID() {
    return std::string("Node_").append(std::to_string(getID()));
}

std::string MeshNode::getAttributeID(Attribute& attr) {
    return getDataTypeName(attr.data).append("Attribute_").append(getNodeID());
}

void MeshNode::resizeAttributes() {
    for (auto& attribute : mAttributes)
        std::visit([this](auto& data) { data.resize(mNumVertices); }, attribute.data);
}

void MeshNode::clearMesh() {
    mNumVertices = 1;
    mNumIndices = 0;

    mType = Mesh::Type::Triangles;

    clearAttributes();
}

void MeshNode::clearAttributes() {
    mAttributes.clear();
    mIndices.clear();
}
