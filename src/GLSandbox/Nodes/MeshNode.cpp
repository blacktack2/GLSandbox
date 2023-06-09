#include "MeshNode.h"

#include "../../Utils/FileUtils.h"
#include "../../Utils/SerializationUtils.h"

#include <filesystem>
#include <unordered_set>

glm::vec4 calculateTangentFromTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
    const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc) {
    glm::vec3 ba = b - a;
    glm::vec3 ca = c - a;

    glm::vec2 tba = tb - ta;
    glm::vec2 tca = tc - ta;

    glm::mat2 texMatrix = glm::inverse(glm::mat2(tba, tca));
    
    glm::vec3 tangent   = ba * texMatrix[0][0] + ca * texMatrix[0][1];
    glm::vec3 bitangent = ba * texMatrix[1][0] + ca * texMatrix[1][1];

    glm::vec3 normal = glm::cross(ba, ca);
    glm::vec3 biCross = glm::cross(tangent, normal);

    float handedness = (glm::dot(biCross, bitangent) > 0.0f) ? 1.0f : -1.0f;

    return glm::vec4(glm::normalize(tangent), handedness);
}

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

    std::filesystem::path fileExtension = mFilepath.extension();
    if (fileExtension == getMeshDefaultExtension())
        writeToFile(filepath);
    data.emplace_back("File", SerializationUtils::serializeData(filepath));

    return data;
}

void MeshNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "File") {
        SerializationUtils::deserializeData(stream, mFilepath);
        loadFromFile();
    }
}

void MeshNode::onDeserialize() {
    uploadMesh();
}

void MeshNode::drawContents() {
    ImGui::BeginDisabled(isLocked());

    drawGlobalParameters();

    if (ImUtils::beginHeader("Attributes", generateNodeLabelID("AttributesHeader"), mShowAttributes)) {
        drawAttributes();
        ImUtils::endHeader();
    }
    drawAddAttributePopup();
    if (ImUtils::button("Upload", generateNodeLabelID("Upload")))
        uploadMesh();

    ImGui::EndDisabled();

    drawMeshStatus();
}

void MeshNode::loadFromFile() {
    if (mFilepath.empty())
        return;
    std::filesystem::path fileExtension = mFilepath.extension();

    clearMesh();

    std::ifstream stream(mFilepath);
    if (!stream)
        return;

    if (fileExtension == ".msh") {
        loadFromStreamMSH(stream);
        mFromOBJ = false;
    } else if (fileExtension == ".obj") {
        loadFromStreamOBJ(stream);
        mFromOBJ = true;
    }
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
            std::string indexData = SerializationUtils::readLine(stream);

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
    posAttr.binding = 0;
    posAttr.data = positions;
    posAttr.name = "position";

    Attribute& uvAttr = mAttributes.emplace_back();
    uvAttr.binding = 1;
    uvAttr.data = uvs;
    uvAttr.name = "uv";

    Attribute& normAttr = mAttributes.emplace_back();
    normAttr.binding = 2;
    normAttr.data = normals;
    normAttr.name = "normal";
}

void MeshNode::loadFromStreamMSH(std::ifstream& stream) {
    SerializationUtils::skipToNextLine(stream);

    stream >> mNumVertices;
    stream >> mNumIndices;

    mIndices.resize(mNumIndices);

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

        if (name == "i") {
            for (unsigned int i = 0; i < mNumIndices; i++)
                stream >> mIndices[i];
            continue;
        }

        unsigned int length;
        char dataType;
        unsigned int binding;
        stream >> length;
        stream >> dataType;
        stream >> binding;

        Attribute& attribute = mAttributes.emplace_back();
        attribute.binding = binding;
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

void MeshNode::writeToFile(const std::filesystem::path& filename) const {
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
        stream << " " << attribute.binding << "\n";

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
    if (mNumIndices > 0) {
        stream << "i\n";
        for (unsigned int index : mIndices) {
            stream << index << "\n";
        }
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
                mMesh->addAttribute(data.data(), 1, sizeof(int), attr.binding, attr.name);
            },
            [&attr, this](const std::vector<float>& data) {
                using type = std::decay_t<decltype(data)>;
                mMesh->addAttribute(data.data(), 1, sizeof(float), attr.binding, attr.name);
            },
            [&attr, this](const auto& data) {
                using type = std::decay_t<decltype(data)>;
                mMesh->addAttribute(data.data(), type::value_type::length(), sizeof(typename type::value_type), attr.binding, attr.name);
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

    if (mFromOBJ && ImUtils::button("Build Tangents", generateNodeLabelID("GenerateTangents")))
        generateTangents();

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

    if (mNumIndices == 0)
        return;

    ImUtils::dataPanelButton("Indices", generateNodeLabelID("Indices"), *this,
        [this]() {
            ImGui::TextUnformatted("Indices");

            ImGui::TextUnformatted("Data:");
            for (unsigned int i = 0; i < mNumIndices; i++)
                ImUtils::inputInt((int*)&mIndices[i], generateNodeLabelID("Index", i), 0, INT_MAX);
        }
    );
}

void MeshNode::drawAttribute(Attribute& attribute) {
    ImUtils::dataPanelButton(attribute.name, generateAttributeLabelID(attribute, "DataPanel"), *this,
        [this, &attribute]() {
            if (ImUtils::button("Remove", generateAttributeLabelID(attribute, "Remove"))) {
                attribute.isMarkedDelete = true;
                ImUtils::softUnsetDataPanel(*this);
                return;
            }

            ImUtils::inputText(attribute.name, generateAttributeLabelID(attribute, "Name"));
            if (ImGui::IsItemHovered())
                ImUtils::postTooltip([]() { ImGui::TextUnformatted("Attribute Name"); });

            ImGui::TextUnformatted("Binding");
            ImUtils::inputInt((int*)&attribute.binding, generateAttributeLabelID(attribute, "Binding"), 0, INT_MAX);

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
        }
    );
}

void MeshNode::drawAddAttributePopup() {
    if (!ImUtils::beginHeader("Add Attribute", generateNodeLabelID("AddAttribute"), mShowAddAttribute))
        return;

    for (unsigned int i = 0; i < (unsigned int)Mesh::AttributeType::Max; i++) {
        Mesh::AttributeType type = (Mesh::AttributeType)i;
        if (!ImUtils::button(getDataTypeName(type), "AttributeButton"))
            continue;

        Attribute& attribute = mAttributes.emplace_back();
        attribute.binding = mAttributes.size();
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

void MeshNode::generateTangents() {
    Attribute* posAttr = nullptr;
    Attribute* uvAttr = nullptr;

    for (auto& attribute : mAttributes) {
        if (attribute.name == "position")
            posAttr = &attribute;
        else if (attribute.name == "uv")
            uvAttr = &attribute;
    }

    if (!posAttr || !uvAttr)
        return;

    const std::vector<glm::vec3>& positions = std::visit(VisitOverload{
        [](const std::vector<glm::vec3>& arg) { return arg; },
        [](const auto& arg) { return std::vector<glm::vec3>{}; },
    }, posAttr->data);
    const std::vector<glm::vec2>& uvs= std::visit(VisitOverload{
        [](const std::vector<glm::vec2>& arg) { return arg; },
        [](const auto& arg) { return std::vector<glm::vec2>{}; },
    }, uvAttr->data);

    std::vector<glm::vec4> tangents(mNumVertices, glm::vec4(0.0f));
    for (size_t i = 0; i < mNumIndices; i += 3) {
        unsigned int ia = mIndices[i    ];
        unsigned int ib = mIndices[i + 1];
        unsigned int ic = mIndices[i + 2];

        glm::vec3 a = positions[ia];
        glm::vec3 b = positions[ib];
        glm::vec3 c = positions[ic];

        glm::vec2 ta = uvs[ia];
        glm::vec2 tb = uvs[ib];
        glm::vec2 tc = uvs[ic];

        glm::vec4 tangent = calculateTangentFromTri(a, b, c, ta, tb, tc);
        tangents[ia] = tangent;
        tangents[ib] = tangent;
        tangents[ic] = tangent;
    }

    Attribute& tanAttr = mAttributes.emplace_back();
    tanAttr.binding = 3;
    tanAttr.data = tangents;
    tanAttr.name = "tangent";

    uploadMesh();

    mFilepath.replace_extension(getMeshDefaultExtension());
    writeToFile(mFilepath);

    mFromOBJ = false;
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
