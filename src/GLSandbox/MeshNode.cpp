#include "MeshNode.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

MeshNode::MeshNode() : Node("Mesh") {

}

void MeshNode::drawContents() {
    drawGlobalParameters();

    drawAttributes<int>(mIntAttributes, [](const std::string& label, int& value) {
        ImGui::InputInt(label.c_str(), &value);
    }, "Integer");
    drawAttributes<glm::ivec2>(mIVec2Attributes, [](const std::string& label, glm::ivec2& value) {
        ImGui::InputInt2(label.c_str(), glm::value_ptr(value));
    }, "IVec2");
    drawAttributes<glm::ivec3>(mIVec3Attributes, [](const std::string& label, glm::ivec3& value) {
        ImGui::InputInt3(label.c_str(), glm::value_ptr(value));
    }, "IVec3");
    drawAttributes<glm::ivec4>(mIVec4Attributes, [](const std::string& label, glm::ivec4& value) {
        ImGui::InputInt4(label.c_str(), glm::value_ptr(value));
    }, "IVec4");

    drawAttributes<float>(mFloatAttributes, [](const std::string& label, float& value) {
        ImGui::InputFloat(label.c_str(), &value);
    }, "Float");
    drawAttributes<glm::vec2>(mVec2Attributes, [](const std::string& label, glm::vec2& value) {
        ImGui::InputFloat2(label.c_str(), glm::value_ptr(value));
    }, "Vec2");
    drawAttributes<glm::vec3>(mVec3Attributes, [](const std::string& label, glm::vec3& value) {
        ImGui::InputFloat3(label.c_str(), glm::value_ptr(value));
    }, "Vec3");
    drawAttributes<glm::vec4>(mVec4Attributes, [](const std::string& label, glm::vec4& value) {
        ImGui::InputFloat4(label.c_str(), glm::value_ptr(value));
    }, "Vec4");

    drawAddAttributePopup();

    drawUploadButton();

    drawMeshStatus();
}

void MeshNode::drawGlobalParameters() {
    const std::string NUM_VERTICES_LABEL = std::string("Num Vertices##NumVertices_").append(getNodeID());
    if (ImGui::InputInt(NUM_VERTICES_LABEL.c_str(), reinterpret_cast<int*>(&mNumVertices))) {
        mNumVertices = std::max(mNumVertices, 1u);
        resizeAttributes();
    }
    const std::string TYPE_LABEL = std::string("Type##Type_").append(getNodeID());
    if (ImGui::BeginCombo(TYPE_LABEL.c_str(), mTypes[mTypeIndex].c_str())) {
        for (size_t i = 0; i < mTypes.size(); i++) {
            const bool isSelected = mTypeIndex == i;
            if (ImGui::Selectable(mTypes[i].c_str(), isSelected)) {
                mTypeIndex = i;
                mType = (Mesh::Type)mTypeIndex;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

template<typename T>
void MeshNode::drawAttributes(std::vector<Attribute<T>>& attributes, draw_attribute_input_callback_t<T> callback, const std::string& typeLabel) {
    for (size_t i = 0; i < attributes.size(); i++) {
        drawAttribute(attributes[i], callback, typeLabel);
    }
    attributes.erase(std::remove_if(attributes.begin(), attributes.end(), [](Attribute<T>& attr) { return attr.remove; }), attributes.end());
}

template<typename T>
void MeshNode::drawAttribute(Attribute<T>& attribute, draw_attribute_input_callback_t<T> callback, const std::string& typeLabel) {
    const std::string ATTRIBUTE_SHOW_LABEL = std::string(attribute.name).append(" (").append(typeLabel).append(")")
            .append("##AttributeHeader_").append(std::to_string(attribute.id)).append(getAttributeID(typeLabel));
    if (ImGui::Button(ATTRIBUTE_SHOW_LABEL.c_str(), ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0)))
        attribute.show = !attribute.show;
    ImGui::SameLine();
    const std::string ATTRIBUTE_REMOVE_LABEL = std::string("Remove")
            .append("##AttributeHeader_").append(std::to_string(attribute.id)).append(getAttributeID(typeLabel));
    if (ImGui::Button(ATTRIBUTE_REMOVE_LABEL.c_str(), ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0))) {
        attribute.remove = true;
        return;
    }
    if (!attribute.show)
        return;

    ImGui::Indent(16.0f);
    const std::string ATTRIBUTE_NAME_LABEL = std::string("name##AttributeName_")
            .append(std::to_string(attribute.id)).append(getAttributeID(typeLabel));
    ImGui::InputText(ATTRIBUTE_NAME_LABEL.c_str(), &attribute.name);

    for (unsigned int i = 0; i < attribute.data.size(); i++) {
        const std::string VALUE_LABEL = std::to_string(i).append("##AttributeValue_")
                .append(std::to_string(attribute.id)).append(getAttributeID(typeLabel));
        callback(VALUE_LABEL, attribute.data[i]);
    }
    ImGui::Unindent(0.0f);
}

void MeshNode::drawAddAttributePopup() {
    const std::string ADD_ATTRIBUTE_LABEL = std::string("Add Attribute##AddAttributeButton_").append(getNodeID());
    const std::string ADD_ATTRIBUTE_POPUP_ID = std::string("AddAttributePopup").append(getNodeID());
    if (ImGui::Button(ADD_ATTRIBUTE_LABEL.c_str()))
        ImGui::OpenPopup(ADD_ATTRIBUTE_POPUP_ID.c_str());

    if (ImGui::BeginPopup(ADD_ATTRIBUTE_POPUP_ID.c_str())) {
        drawAttributeSelection(mIntAttributes, "Integer");
        drawAttributeSelection(mIVec2Attributes, "IVec2");
        drawAttributeSelection(mIVec3Attributes, "IVec3");
        drawAttributeSelection(mIVec4Attributes, "IVec4");
        drawAttributeSelection(mFloatAttributes, "Float");
        drawAttributeSelection(mVec2Attributes, "Vec2");
        drawAttributeSelection(mVec3Attributes, "Vec3");
        drawAttributeSelection(mVec4Attributes, "Vec4");

        ImGui::EndPopup();
    }
}

template<typename T>
void MeshNode::drawAttributeSelection(std::vector<Attribute<T>>& attrVec, const std::string& typeLabel) {
    static unsigned int sAttributeID = 0;
    const std::string INT_ATTRIBUTE_ID = std::string(typeLabel).append("##Selectable_").append(getAttributeID(typeLabel));
    if (ImGui::Selectable(INT_ATTRIBUTE_ID.c_str())) {
        auto& attr = attrVec.emplace_back();
        attr.id = sAttributeID++;
        attr.data.resize(mNumVertices);
    }
}

void MeshNode::drawUploadButton() {
    const std::string UPLOAD_BUTTON_LABEL = std::string("Upload##UploadButton_").append(getNodeID());
    if (ImGui::Button(UPLOAD_BUTTON_LABEL.c_str())) {
        mMesh.hardClean();
        mMesh.setNumVertices(mNumVertices);
        for (const auto& attr : mIntAttributes)
            mMesh.addAttribute(attr.data.data(), 1, sizeof(int), attr.name);
        for (const auto& attr : mIVec2Attributes)
            mMesh.addAttribute(attr.data.data(), 2, sizeof(glm::ivec2), attr.name);
        for (const auto& attr : mIVec3Attributes)
            mMesh.addAttribute(attr.data.data(), 3, sizeof(glm::ivec3), attr.name);
        for (const auto& attr : mIVec4Attributes)
            mMesh.addAttribute(attr.data.data(), 4, sizeof(glm::ivec4), attr.name);

        for (const auto& attr : mFloatAttributes)
            mMesh.addAttribute(attr.data.data(), 1, sizeof(float), attr.name);
        for (const auto& attr : mVec2Attributes)
            mMesh.addAttribute(attr.data.data(), 2, sizeof(glm::vec2), attr.name);
        for (const auto& attr : mVec3Attributes)
            mMesh.addAttribute(attr.data.data(), 3, sizeof(glm::vec3), attr.name);
        for (const auto& attr : mVec4Attributes)
            mMesh.addAttribute(attr.data.data(), 4, sizeof(glm::vec4), attr.name);

        mMesh.bufferData();
    }
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

std::string MeshNode::getAttributeID(const std::string& typeLabel) {
    return std::string(typeLabel).append("Attribute_").append(getNodeID());
}

void MeshNode::resizeAttributes() {
    for (auto& attr : mIntAttributes)
        attr.data.resize(mNumVertices, 0);
    for (auto& attr : mIVec2Attributes)
        attr.data.resize(mNumVertices, glm::ivec2(0));
    for (auto& attr : mIVec3Attributes)
        attr.data.resize(mNumVertices, glm::ivec3(0));
    for (auto& attr : mIVec4Attributes)
        attr.data.resize(mNumVertices, glm::ivec4(0));

    for (auto& attr : mFloatAttributes)
        attr.data.resize(mNumVertices, 0.0f);
    for (auto& attr : mVec2Attributes)
        attr.data.resize(mNumVertices, glm::vec2(0.0f));
    for (auto& attr : mVec3Attributes)
        attr.data.resize(mNumVertices, glm::vec3(0.0f));
    for (auto& attr : mVec4Attributes)
        attr.data.resize(mNumVertices, glm::vec4(0.0f));
}
