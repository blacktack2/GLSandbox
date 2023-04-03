#include "MeshNode.h"

#include "../Assets.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <imnodes.h>

#include <filesystem>
#include <regex>
#include <unordered_set>

static const std::unordered_map<std::type_index, std::string> gTYPE_NAMES = {
    {typeid(int),        "Int"},
    {typeid(glm::ivec2), "IVec2"},
    {typeid(glm::ivec3), "IVec3"},
    {typeid(glm::ivec4), "IVec4"},
    {typeid(float),      "Float"},
    {typeid(glm::vec2),  "Vec2"},
    {typeid(glm::vec3),  "Vec3"},
    {typeid(glm::vec4),  "Vec4"},
};
static const std::unordered_map<std::type_index, std::function<void(const char* label, void* data)>> gTYPE_DISPLAY_CALLBACKS = {
    {typeid(int),        [](const char* label, void* data) { ImGui::InputInt(label, (int*)data); }},
    {typeid(glm::ivec2), [](const char* label, void* data) { ImGui::InputInt2(label, (int*)data); }},
    {typeid(glm::ivec3), [](const char* label, void* data) { ImGui::InputInt3(label, (int*)data); }},
    {typeid(glm::ivec4), [](const char* label, void* data) { ImGui::InputInt4(label, (int*)data); }},
    {typeid(float),      [](const char* label, void* data) { ImGui::InputFloat(label, (float*)data); }},
    {typeid(glm::vec2),  [](const char* label, void* data) { ImGui::InputFloat2(label, (float*)data); }},
    {typeid(glm::vec3),  [](const char* label, void* data) { ImGui::InputFloat3(label, (float*)data); }},
    {typeid(glm::vec4),  [](const char* label, void* data) { ImGui::InputFloat4(label, (float*)data); }},
};
static const std::unordered_map<std::type_index, size_t> gTYPE_SIZES = {
    {typeid(int),        sizeof(int)},
    {typeid(glm::ivec2), sizeof(glm::ivec2)},
    {typeid(glm::ivec3), sizeof(glm::ivec3)},
    {typeid(glm::ivec4), sizeof(glm::ivec4)},
    {typeid(float),      sizeof(float)},
    {typeid(glm::vec2),  sizeof(glm::vec2)},
    {typeid(glm::vec3),  sizeof(glm::vec3)},
    {typeid(glm::vec4),  sizeof(glm::vec4)},
};
static const std::unordered_map<std::type_index, unsigned int> gTYPE_ATTR_COUNTS = {
        {typeid(int),        1},
        {typeid(glm::ivec2), 2},
        {typeid(glm::ivec3), 3},
        {typeid(glm::ivec4), 4},
        {typeid(float),      1},
        {typeid(glm::vec2),  2},
        {typeid(glm::vec3),  3},
        {typeid(glm::vec4),  4},
};

MeshNode::MeshNode() : Node("Mesh") {
    mAttributes.emplace(typeid(int),        std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(glm::ivec2), std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(glm::ivec3), std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(glm::ivec4), std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(float),      std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(glm::vec2),  std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(glm::vec3),  std::vector<std::unique_ptr<IAttribute>>());
    mAttributes.emplace(typeid(glm::vec4),  std::vector<std::unique_ptr<IAttribute>>());

    addPort(mMeshOutPort);
}

bool MeshNode::isValid() const {
    return mMesh.getState() == Mesh::ErrorState::VALID;
}

void MeshNode::clearAttributes() {
    for (auto& attrPair : mAttributes)
        attrPair.second.clear();
    mIndices.clear();
}

void MeshNode::serializeContents(std::ofstream& streamOut) const {
    if (mFilename.empty())
        const_cast<MeshNode*>(this)->generateFilename();
    streamOut << mFilename << "\n";
    streamOut << (unsigned int)mType << "\n";
    streamOut << mNumVertices << "\n";
    streamOut << mNumIndices << "\n";

    std::ofstream meshStream(std::string(gMESH_ASSET_DIR).append(mFilename).append(gMESH_ASSET_EXTENSION));
    if (meshStream)
        writeToStreamOBJEXT(meshStream);
}

void MeshNode::deserializeContents(std::ifstream& streamIn) {
    streamIn >> mFilename;
    unsigned int type;
    streamIn >> type;
    mType = (Mesh::Type)type;
    streamIn >> mNumVertices;
    streamIn >> mNumIndices;

    std::ifstream meshStream(std::string(gMESH_ASSET_DIR).append(mFilename).append(gMESH_ASSET_EXTENSION));
    if (meshStream)
        loadFromStreamOBJEXT(meshStream);
}

void MeshNode::drawContents() {
    drawGlobalParameters();

    for (auto& attrPair : mAttributes)
        drawAttributes(attrPair.second, attrPair.first);

    drawAddAttributePopup();

    drawUploadButton();

    drawMeshStatus();
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
            positions.push_back(vertex);
        } else if (type == "vt") {
            glm::vec2 uv;
            stream >> uv.x;
            stream >> uv.y;
            uvs.push_back(uv);
        } else if (type == "vn") {
            glm::vec3 normal;
            stream >> normal.x;
            stream >> normal.y;
            stream >> normal.z;
            normals.push_back(normal);
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
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
    }

    clearAttributes();

    mNumVertices = positions.size();
    mNumIndices = indices.size();

    mType = Mesh::Type::Triangles;

    mIndices = indices;

    std::unique_ptr<IAttribute> posAttr = mAttributeFactories.find(typeid(glm::vec3))->second();
    dynamic_cast<Attribute<glm::vec3>*>(posAttr.get())->setData(positions);
    posAttr->setName("position");

    std::unique_ptr<IAttribute> uvAttr = mAttributeFactories.find(typeid(glm::vec2))->second();
    dynamic_cast<Attribute<glm::vec2>*>(uvAttr.get())->setData(uvs);
    uvAttr->setName("uv");

    std::unique_ptr<IAttribute> normAttr = mAttributeFactories.find(typeid(glm::vec3))->second();
    dynamic_cast<Attribute<glm::vec3>*>(normAttr.get())->setData(normals);
    normAttr->setName("normal");
}

void MeshNode::loadFromStreamOBJEXT(std::ifstream& stream) {
    clearAttributes();
    while (!stream.eof()) {
        std::string name;
        stream >> name;
        if (name.empty() || name[0] == '#') {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        unsigned int length;
        char type;
        stream >> length;
        stream >> type;

        std::type_index attrType = typeid(void);
        switch (type) {
            case 'i':
                switch (length) {
                    case 1: attrType = typeid(int); break;
                    case 2: attrType = typeid(glm::ivec2); break;
                    case 3: attrType = typeid(glm::ivec3); break;
                    case 4: attrType = typeid(glm::ivec4); break;
                    default:
                        stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                }
                break;
            default:
            case 'f':
                switch (length) {
                    case 1: attrType = typeid(float); break;
                    case 2: attrType = typeid(glm::vec2); break;
                    case 3: attrType = typeid(glm::vec3); break;
                    case 4: attrType = typeid(glm::vec4); break;
                    default:
                        stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        continue;
                }
                break;
        }
        std::unique_ptr<IAttribute> attribute = mAttributeFactories.find(attrType)->second();

        attribute->setName(name);
        attribute->resizeData(mNumVertices);
        attribute->deserialize(stream);

        mAttributes.find(attrType)->second.push_back(std::move(attribute));
    }
}

void MeshNode::writeToStreamOBJEXT(std::ofstream& stream) const {
    stream << "# GLSandbox OBJEXT File: " << mFilename << "\n";

    for (const auto& attrPair : mAttributes)
        for (const auto& attr : attrPair.second)
            attr->serialize(stream);
    for (unsigned int index : mIndices) {
        stream << "i " << index << "\n";
    }
}

void MeshNode::generateFilename() {
    static const std::regex cDEFAULT_FILE_REGEX("^Mesh(0-9)+$");

    std::unordered_set<unsigned int> paths;
    for (auto const& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(gMESH_ASSET_DIR))) {
        if (!(std::filesystem::is_regular_file(entry) && entry.path().extension() == gMESH_ASSET_EXTENSION))
            continue;
        std::smatch matches;
        std::string filename = entry.path().filename().string();
        if (std::regex_search(filename, matches, cDEFAULT_FILE_REGEX))
            paths.emplace(std::stoi(matches[1]));
    }
    unsigned int counter = 0;
    while (paths.contains(++counter)) {}

    mFilename = std::string("Mesh").append(std::to_string(counter));
}

void MeshNode::uploadMesh() {
    mMesh.hardClean();
    mMesh.setType(mType);
    mMesh.setNumVertices(mNumVertices);
    for (const auto& attrPair : mAttributes)
        for (const auto& attr : attrPair.second)
            mMesh.addAttribute(attr->getData(), gTYPE_ATTR_COUNTS.find(attrPair.first)->second,
                               gTYPE_SIZES.find(attrPair.first)->second, attr->getName());

    mMesh.bufferData();
}

void MeshNode::drawGlobalParameters() {
    const std::string NUM_VERTICES_LABEL = std::string("Num Vertices##NumVertices_").append(getNodeID());
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::InputInt(NUM_VERTICES_LABEL.c_str(), reinterpret_cast<int*>(&mNumVertices))) {
        mNumVertices = std::max(mNumVertices, 1u);
        resizeAttributes();
    }
    const std::string TYPE_LABEL = std::string("Type##Type_").append(getNodeID());
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

void MeshNode::drawAttributes(std::vector<std::unique_ptr<IAttribute>>& attributes, std::type_index type) {
    for (auto& attribute : attributes) {
        drawAttribute(*attribute, type);
    }
    attributes.erase(std::remove_if(attributes.begin(), attributes.end(),
                                    [](auto& attr) { return attr->isMarkedDelete(); }), attributes.end());
}

void MeshNode::drawAttribute(IAttribute& attribute, std::type_index type) {
    const std::string ATTRIBUTE_SHOW_LABEL = std::string(attribute.getName()).append(" (")
            .append(gTYPE_NAMES.find(type)->second).append(")")
            .append("##AttributeHeader_").append(std::to_string(attribute.getID())).append(getAttributeID(type));
    const float BUTTON_SIZE = 200.0f;
    if (ImGui::Button(ATTRIBUTE_SHOW_LABEL.c_str(), ImVec2(BUTTON_SIZE, 0)))
        attribute.setShown(!attribute.isShown());
    ImGui::SameLine();
    const std::string ATTRIBUTE_REMOVE_LABEL = std::string("Remove")
            .append("##AttributeHeader_").append(std::to_string(attribute.getID())).append(getAttributeID(type));
    if (ImGui::Button(ATTRIBUTE_REMOVE_LABEL.c_str(), ImVec2(BUTTON_SIZE, 0))) {
        attribute.markForDeletion();
        return;
    }
    if (!attribute.isShown())
        return;

    ImGui::Indent(16.0f);
    const std::string ATTRIBUTE_NAME_LABEL = std::string("name##AttributeName_")
            .append(std::to_string(attribute.getID())).append(getAttributeID(type));
    std::string name = attribute.getName();
    if (ImGui::InputText(ATTRIBUTE_NAME_LABEL.c_str(), &name))
        attribute.setName(name);

    for (unsigned int i = 0; i < mNumVertices; i++) {
        const std::string VALUE_LABEL = std::to_string(i).append("##AttributeValue_")
                .append(std::to_string(attribute.getID())).append(getAttributeID(type));
        gTYPE_DISPLAY_CALLBACKS.find(type)->second(VALUE_LABEL.c_str(), attribute.getDataAt(i));
    }
    ImGui::Unindent(0.0f);
}

void MeshNode::drawAddAttributePopup() {
    const std::string ADD_ATTRIBUTE_LABEL = std::string("Add Attribute##AddAttributeButton_").append(getNodeID());
    const std::string ADD_ATTRIBUTE_POPUP_ID = std::string("AddAttributePopup").append(getNodeID());
    if (ImGui::Button(ADD_ATTRIBUTE_LABEL.c_str()))
        ImGui::OpenPopup(ADD_ATTRIBUTE_POPUP_ID.c_str());

    if (ImGui::BeginPopup(ADD_ATTRIBUTE_POPUP_ID.c_str())) {
        for (auto& attrPair : mAttributes)
            drawAttributeSelection(attrPair.second, attrPair.first);

        ImGui::EndPopup();
    }
}

void MeshNode::drawAttributeSelection(std::vector<std::unique_ptr<IAttribute>>& attrVec, std::type_index type) {
    const std::string INT_ATTRIBUTE_ID = std::string(gTYPE_NAMES.find(type)->second).append("##Selectable_")
            .append(getAttributeID(type));
    if (ImGui::Selectable(INT_ATTRIBUTE_ID.c_str())) {
        std::unique_ptr<IAttribute> attr = mAttributeFactories.find(type)->second();
        attr->resizeData(mNumVertices);
        attrVec.push_back(std::move(attr));
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
    switch (mMesh.getState()) {
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

std::string MeshNode::getAttributeID(std::type_index type) {
    return std::string(type.name()).append("Attribute_").append(getNodeID());
}

void MeshNode::resizeAttributes() {
    for (auto& attrSet : mAttributes)
        for (auto& attr : attrSet.second)
            attr->resizeData(mNumVertices);
}
