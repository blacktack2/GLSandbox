#include "MeshNode.h"

#include "../../Utils/FileUtils.h"
#include "../../Utils/SerializationUtils.h"

#include <filesystem>
#include <regex>
#include <unordered_set>

MeshNode::MeshNode() : Node("Mesh") {
    mMesh = std::make_unique<Mesh>();

    addPort(mMeshOut);

    mFilepath = generateFilename();
}

std::vector<std::pair<std::string, std::string>> MeshNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    if (!mCanWriteToFile)
        return data;

    std::filesystem::path filepath = mFilepath;
    if (filepath.empty())
        filepath = generateFilename();

    std::string fileExtension = mFilepath.extension();
    if (fileExtension == getMeshDefaultExtension())
        writeToFile(filepath);
    data.emplace_back("File", filepath.string());

    return data;
}

void MeshNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "File") {
        mFilepath = SerializationUtils::readLine(stream);
        loadFromFile();
    }
}

void MeshNode::onDeserialize() {
    uploadMesh();
}

void MeshNode::drawContents() {
    drawGlobalParameters();

    if (ImUtils::beginHeader("Attributes", generateNodeLabelID("AttributesHeader"), mShowAttributes)) {
        drawAttributes();
        ImUtils::endHeader();
    }
    drawAddAttributePopup();
    if (ImUtils::button("Upload", generateNodeLabelID("Upload")))
        uploadMesh();

    drawMeshStatus();
}

void MeshNode::loadFromFile() {
    if (mFilepath.empty())
        return;
    std::string fileExtension = mFilepath.extension();

    clearMesh();

    std::ifstream stream(mFilepath);
    if (!stream)
        return;

    if (fileExtension == ".msh")
        loadFromStreamMSH(stream);
    else if (fileExtension == ".obj")
        loadFromStreamOBJ(stream);
}

void MeshNode::loadFromStreamOBJ(std::ifstream& stream) {
    std::vector<unsigned int> indices;
    std::vector<unsigned int> texIndices;
    std::vector<unsigned int> normIndices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvsSoft;
    std::vector<glm::vec3> normalsSoft;

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
            uvsSoft.emplace_back(uv);
            SerializationUtils::skipToNextLine(stream);
        } else if (type == "vn") {
            glm::vec3 normal;
            stream >> normal.x;
            stream >> normal.y;
            stream >> normal.z;
            normalsSoft.emplace_back(normal);
            SerializationUtils::skipToNextLine(stream);
        } else if (type == "f") {
            std::string indexData;
            std::getline(stream, indexData);

            std::stringstream indexSetStream(indexData);
            indexSetStream >> std::ws;

            std::string indexSet;
            std::vector<unsigned int> tempVis, tempTis, tempNis;
            while (std::getline(indexSetStream, indexSet, ' ')) {
                std::stringstream indexStream(indexSet);
                int vi, ti, ni;
                char sep1, sep2;
                indexStream >> vi;

                tempVis.push_back(vi < 0 ? positions.size() + vi : vi - 1);
                if (indexStream.peek() != '/')
                    continue;

                indexStream >> sep1;
                if (indexStream.peek() != '/') {
                    indexStream >> ti;
                    tempTis.push_back(ti < 0 ? uvsSoft.size() + ti : ti - 1);
                }

                if (indexStream.peek() != '/')
                    continue;
                indexStream >> sep2;
                indexStream >> ni;
                tempNis.push_back(ni < 0 ? normalsSoft.size() + ni : ni - 1);
            }
            if (tempVis.size() <= 3) {
                for (unsigned int index : tempVis)
                    indices.push_back(index);
            } else {
                for (size_t i = 2; i < tempVis.size(); i++) {
                    indices.push_back(tempVis[0]);
                    indices.push_back(tempVis[i - 1]);
                    indices.push_back(tempVis[i    ]);
                }
            }
            if (tempTis.size() <= 3) {
                for (unsigned int index : tempTis)
                    texIndices.push_back(index);
            } else {
                for (size_t i = 2; i < tempTis.size(); i++) {
                    texIndices.push_back(tempTis[0]);
                    texIndices.push_back(tempTis[i - 1]);
                    texIndices.push_back(tempTis[i    ]);
                }
            }
            if (tempNis.size() <= 3) {
                for (unsigned int index : tempNis)
                    normIndices.push_back(index);
            } else {
                for (size_t i = 2; i < tempNis.size(); i++) {
                    normIndices.push_back(tempNis[0]);
                    normIndices.push_back(tempNis[i - 1]);
                    normIndices.push_back(tempNis[i    ]);
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

    std::vector<glm::vec2> uvs(mNumVertices);
    for (size_t i = 0; i < indices.size(); i++) {
        unsigned int vi = indices[i];
        unsigned int ti = texIndices[i];
        uvs[vi] = uvsSoft[ti];
    }

    std::vector<glm::vec3> normals(mNumVertices);
    for (size_t i = 0; i < indices.size(); i++) {
        unsigned int vi = indices[i];
        unsigned int ni = normIndices[i];
        normals[vi] = normalsSoft[ni];
    }

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
    stream << "# GLSandbox MSH File: " << mFilepath << "\n";

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

std::filesystem::path MeshNode::generateFilename() {
    return SerializationUtils::generateFilename(getMeshAssetDirectory(), "Mesh", getMeshDefaultExtension());
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
    ImUtils::fileChooseDialog(mFilepath, getMeshAssetDirectory(), generateNodeLabelID("FileChoose"),
                              getValidMeshFileExtensions());

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
    ImUtils::dataPanelButton(attribute.name, generateAttributeLabelID(attribute, "DataPanel"), *this,
                             [this, &attribute]() {
        if (ImUtils::button("Remove", generateAttributeLabelID(attribute, "Remove"))) {
            attribute.isMarkedDelete = true;
            return;
        }

        ImUtils::inputText(attribute.name, generateAttributeLabelID(attribute, "Name"));
        if (ImGui::IsItemHovered())
            ImUtils::postTooltip([]() { ImGui::TextUnformatted("Attribute Name"); });

        ImGui::TextUnformatted("Data:");
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
    });
}

void MeshNode::drawAddAttributePopup() {
    if (!ImUtils::beginHeader("Add Attribute", generateNodeLabelID("AddAttribute"), mShowAddAttribute))
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
