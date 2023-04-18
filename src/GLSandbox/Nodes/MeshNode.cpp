#include "MeshNode.h"

#include "../../Utils/FileUtils.h"

#include <filesystem>
#include <regex>
#include <unordered_set>

MeshNode::MeshNode() : Node("Mesh") {
    mMesh = std::make_unique<Mesh>();

    addPort(mMeshOut);

    mFilename = generateFilename();
}

std::vector<std::pair<std::string, std::string>> MeshNode::generateSerializedData() const {
    if (!mCanWriteToFile)
        return {};

    std::string filename = mFilename;
    if (filename.empty())
        filename = generateFilename();

    std::size_t index = mFilename.find_last_of('.');
    std::string fileExtension = mFilename.substr(index);
    if (parseFileExtension(fileExtension.c_str()) == gMESH_DEFAULT_EXTENSION)
        writeToFile(filename);

    return {
        {"File", filename},
    };
}

void MeshNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "File") {
        stream >> mFilename;
        loadFromFile();
    }
}

void MeshNode::onDeserialize() {
    uploadMesh();
}

void MeshNode::drawContents() {
    drawGlobalParameters();

    if (ImUtils::beginHeader("Attributes", generateNodeLabelID("AttributesHeader"))) {
        drawAttributes();
        drawAddAttributePopup();
        ImUtils::endHeader();
    }
    if (ImUtils::button("Upload", generateNodeLabelID("Upload")))
        uploadMesh();

    drawMeshStatus();
}

void MeshNode::loadFromFile() {
    std::size_t index = mFilename.find_last_of('.');
    std::string fileExtension = mFilename.substr(index);

    clearMesh();

    std::ifstream stream(mFilename);
    if (!stream)
        return;
    switch (parseFileExtension(fileExtension.c_str())) {
        case MeshFileExtension::MSH: loadFromStreamMSH(stream); break;
        case MeshFileExtension::OBJ: loadFromStreamOBJ(stream); break;
        default: break;
    }
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
            SerializationUtils::skipToNextLine(stream);
        } else if (type == "vt") {
            glm::vec2 uv;
            stream >> uv.x;
            stream >> uv.y;
            uvs.emplace_back(uv);
            SerializationUtils::skipToNextLine(stream);
        } else if (type == "vn") {
            glm::vec3 normal;
            stream >> normal.x;
            stream >> normal.y;
            stream >> normal.z;
            normals.emplace_back(normal);
            SerializationUtils::skipToNextLine(stream);
        } else if (type == "f") {
            std::string indexData;
            std::getline(stream, indexData);

            std::stringstream indexSetStream(indexData);
            indexSetStream >> std::ws;
            std::string indexSet;
            std::vector<unsigned int> tempIndices;
            while (std::getline(indexSetStream, indexSet, ' ')) {
                std::stringstream indexStream(indexSet);
                unsigned int index;
                indexStream >> index;
                tempIndices.push_back(index - 1);
            }
            if (tempIndices.size() <= 3) {
                for (unsigned int index : tempIndices)
                    indices.push_back(index);
            } else {
                for (size_t i = 2; i < tempIndices.size(); i++) {
                    indices.push_back(tempIndices[0]);
                    indices.push_back(tempIndices[i - 1]);
                    indices.push_back(tempIndices[i    ]);
                }
            }
            SerializationUtils::skipToNextLine(stream);
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

    Attribute& posAttr = mAttributes.emplace_back();
    posAttr.data = positions;
    posAttr.name = "position";

    Attribute& uvAttr = mAttributes.emplace_back();
    uvAttr.data = uvs;
    uvAttr.name = "uv";

    Attribute& normAttr = mAttributes.emplace_back();
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

void MeshNode::writeToFile(const std::string& filename) const {
    std::ofstream stream(filename);
    writeToStreamMSH(stream);
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
            parseFileExtension(entry.path().extension().string().c_str()) == MeshFileExtension::Undefined)
            continue;
        std::smatch matches;
        std::string filename = entry.path().filename().string();
        if (std::regex_search(filename, matches, cDEFAULT_FILE_REGEX))
            paths.emplace(std::stoi(matches[1]));
    }
    unsigned int counter = 0;
    while (paths.contains(++counter)) {}

    return std::string("Mesh").append(std::to_string(counter)).append(getFileExtension(gMESH_DEFAULT_EXTENSION));
}

void MeshNode::uploadMesh() {
    mMesh->hardClean();
    mMesh->setType(mType);
    mMesh->setNumVertices(mNumVertices);
    mMesh->setIndices(mIndices);
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
    std::size_t index = mFilename.find_last_of('/');
    ImGui::Text("%s", mFilename.substr(index + 1).c_str());

    ImUtils::fileChooseDialog(mFilename, gMESH_ASSET_DIR, generateNodeLabelID("FileChoose"));

    if (ImUtils::button("Load", generateNodeLabelID("LoadButton")))
        loadFromFile();

    ImGui::Text("Vertex Count:");
    if (ImUtils::inputInt((int*)&mNumVertices, generateNodeLabelID("NumVertices"), 1, INT_MAX)) {
        mNumVertices = std::max(mNumVertices, 1u);
        resizeAttributes();
    }

    ImGui::Text("Primitive Type:");
    ImUtils::cycleButton(generateNodeLabelID("PrimitiveType"), (size_t&)mType, (size_t)Mesh::Type::Max,
                         [](size_t index) { return getPrimitiveTypeLabel((Mesh::Type)index); });
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
    const std::string headerDisplayText = std::string(attribute.name).append(" (")
        .append(getDataTypeName(attribute.data)).append(")");
    if (!ImUtils::beginHeader(headerDisplayText, generateAttributeLabelID(attribute, "Header")))
        return;

    if (ImUtils::button("Remove", generateAttributeLabelID(attribute, "Remove"))) {
        attribute.isMarkedDelete = true;
        return;
    }

    ImGui::Text("Name");
    ImUtils::inputText(attribute.name, generateAttributeLabelID(attribute, "Name"));

    const std::string attributeAreaLabel = generateAttributeLabelID(attribute, "ChildWindow");
    ImGui::BeginChild(attributeAreaLabel.c_str(), ImVec2(100, 100), true);

    for (unsigned int i = 0; i < mNumVertices; i++) {
        const std::string valueInputLabel = generateAttributeLabelID(attribute, "Value", std::to_string(i));
        std::visit(VisitOverload{
            [valueInputLabel, i](std::vector<int>& data) {
                ImUtils::inputIntN(&data[i], 1, valueInputLabel);
            },
            [valueInputLabel, i](std::vector<glm::ivec2>& data) {
                ImUtils::inputIntN(&data[i][0], 2, valueInputLabel);
            },
            [valueInputLabel, i](std::vector<glm::ivec3>& data) {
                ImUtils::inputIntN(&data[i][0], 3, valueInputLabel);
            },
            [valueInputLabel, i](std::vector<glm::ivec4>& data) {
                ImUtils::inputIntN(&data[i][0], 4, valueInputLabel);
            },

            [valueInputLabel, i](std::vector<float>& data) {
                ImUtils::inputFloatN(&data[i], 1, valueInputLabel);
            },
            [valueInputLabel, i](std::vector<glm::vec2>& data) {
                ImUtils::inputFloatN(&data[i][0], 2, valueInputLabel);
            },
            [valueInputLabel, i](std::vector<glm::vec3>& data) {
                ImUtils::inputFloatN(&data[i][0], 3, valueInputLabel);
            },
            [valueInputLabel, i](std::vector<glm::vec4>& data) {
                ImUtils::inputFloatN(&data[i][0], 4, valueInputLabel);
            },

            [](auto arg) { ImGui::Text("Undefined"); },
        }, attribute.data);
    }

    ImGui::EndChild();
    ImUtils::endHeader();
}

void MeshNode::drawAddAttributePopup() {
    if (!ImUtils::beginHeader("Add Attribute", generateNodeLabelID("AddAttribute")))
        return;

    for (unsigned int i = 0; i < (unsigned int)Mesh::AttributeType::Max; i++) {
        Mesh::AttributeType type = (Mesh::AttributeType)i;
        if (!ImUtils::button(getDataTypeName(type), "AttributeButton"))
            continue;

        Attribute& attribute = mAttributes.emplace_back();
        attribute.data = generateAttributeDataset(type);
        std::visit([this](auto& data) { data.resize(mNumVertices); }, attribute.data);
    }

    ImUtils::endHeader();
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

    drawMessage(text, colour);
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
