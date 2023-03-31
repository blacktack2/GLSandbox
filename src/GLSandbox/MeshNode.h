#pragma once
#include "../NodeEditor/Node.h"
#include "../Rendering/Mesh.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <any>
#include <functional>
#include <string>
#include <vector>

class MeshNode : public Node {
public:
    MeshNode();
protected:
    void drawContents() override;
private:
    template<typename T>
    using draw_attribute_input_callback_t = std::function<void(const std::string&, T&)>;

    template<typename T>
    struct Attribute {
        unsigned int id = 0;
        std::string name;
        std::vector<T> data{};
        bool show = false;
        bool remove = false;
    };

    void drawGlobalParameters();

    template<typename T>
    void drawAttributes(std::vector<Attribute<T>>& attributes, draw_attribute_input_callback_t<T> callback, const std::string& typeLabel);
    template<typename T>
    void drawAttribute(Attribute<T>& attribute, draw_attribute_input_callback_t<T> callback, const std::string& typeLabel);

    void drawAddAttributePopup();
    template<typename T>
    void drawAttributeSelection(std::vector<Attribute<T>>& attrVec, const std::string& typeLabel);

    void drawUploadButton();

    void drawMeshStatus();

    std::string getNodeID();
    std::string getAttributeID(const std::string& typeLabel);

    void resizeAttributes();

    Mesh mMesh;
    OutPort mMeshOutPort = OutPort(*this, "Mesh");

    unsigned int mNumVertices = 1;
    unsigned int mNumIndices = 0;

    Mesh::Type mType = Mesh::Type::Triangles;

    std::vector<Attribute<int>> mIntAttributes{};
    std::vector<Attribute<glm::ivec2>> mIVec2Attributes{};
    std::vector<Attribute<glm::ivec3>> mIVec3Attributes{};
    std::vector<Attribute<glm::ivec4>> mIVec4Attributes{};
    std::vector<Attribute<float>> mFloatAttributes{};
    std::vector<Attribute<glm::vec2>> mVec2Attributes{};
    std::vector<Attribute<glm::vec3>> mVec3Attributes{};
    std::vector<Attribute<glm::vec4>> mVec4Attributes{};
    std::vector<GLuint> mIndices{};

    size_t mTypeIndex = (size_t)mType;

    // Matches Mesh::Type
    const std::vector<std::string> mTypes {
        "Triangles",
        "Triangle Strip",
        "Triangle Fan",
        "Lines",
        "Points",
    };
};

