#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Mesh.h"

#include "../../Utils/VariantUtils.h"

#include "../Assets.h"
#include "../NodeClassifications.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <any>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class MeshNode final : public Node {
public:
    MeshNode();
    ~MeshNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Mesh;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final;

    void drawContents() override;
private:
    struct Attribute {
        Attribute() {
            static unsigned int cIDCounter = 0;
            id = cIDCounter++;
        }

        unsigned int id;

        std::string name = "unnamed";

        VectorVariant<Mesh::attribute_t> data;

        bool isMarkedDelete = false;
        bool show = false;
    };

    void loadFromFile();
    void loadFromStreamOBJ(std::ifstream& stream);
    void loadFromStreamMSH(std::ifstream& stream);
    void writeToFile(const std::string& filename) const;
    void writeToStreamMSH(std::ofstream& stream) const;

    [[nodiscard]] static std::filesystem::path generateFilename() ;

    void uploadMesh();

    void drawGlobalParameters();

    void drawAttributes();
    void drawAttribute(Attribute& attribute);

    void drawAddAttributePopup();

    void drawMeshStatus();

    void resizeAttributes();

    void clearMesh();
    void clearAttributes();

    template<typename... Args>
    inline std::string generateAttributeLabelID(const Attribute& attribute, Args... args) const {
        return generateNodeLabelID("Attribute", attribute.id, args...);
    }

    static inline std::string getDataTypeName(Mesh::AttributeType type) {
        switch (type) {
            case Mesh::AttributeType::Integer : return "Int";
            case Mesh::AttributeType::IVec2   : return "IVec2";
            case Mesh::AttributeType::IVec3   : return "IVec3";
            case Mesh::AttributeType::IVec4   : return "IVec4";

            case Mesh::AttributeType::Float   : return "Float";
            case Mesh::AttributeType::Vec2    : return "Vec2";
            case Mesh::AttributeType::Vec3    : return "Vec3";
            case Mesh::AttributeType::Vec4    : return "Vec4";

            default: return "UNDEFINED";
        }
    }
    static inline std::string getDataTypeName(const VectorVariant<Mesh::attribute_t>& attr) {
        return std::visit(VisitOverload{
            [](const auto& arg                   ) { return "UNDEFINED"; },

            [](const std::vector<int>& arg       ) { return "Int";   },
            [](const std::vector<glm::ivec2>& arg) { return "IVec2"; },
            [](const std::vector<glm::ivec3>& arg) { return "IVec3"; },
            [](const std::vector<glm::ivec4>& arg) { return "IVec4"; },

            [](const std::vector<float>& arg     ) { return "Float"; },
            [](const std::vector<glm::vec2>& arg ) { return "Vec2";  },
            [](const std::vector<glm::vec3>& arg ) { return "Vec3";  },
            [](const std::vector<glm::vec4>& arg ) { return "Vec4";  },
        }, attr);
    }
    static inline std::size_t getAttrSize(Mesh::attribute_t attr) {
        return std::visit(VisitOverload{
            [](int arg       ) { return 1; },
            [](float arg     ) { return 1; },
            [](auto arg     ) { return arg.length(); },
        }, attr);
    }
    static inline std::size_t getAttrDataSize(Mesh::attribute_t attr) {
        return std::visit(VisitOverload{
            [](const auto& arg) { return sizeof(std::decay_t<decltype(arg)>); },
        }, attr);
    }
    static inline VectorVariant<Mesh::attribute_t> createAttributeDataset(unsigned int length, char dataType) {
        switch (dataType) {
            case 'i':
                switch (length) {
                    default:
                    case 1: return std::vector<int>();
                    case 2: return std::vector<glm::ivec2>();
                    case 3: return std::vector<glm::ivec3>();
                    case 4: return std::vector<glm::ivec4>();
                }
            default:
            case 'f':
                switch (length) {
                    default:
                    case 1: return std::vector<float>();
                    case 2: return std::vector<glm::vec2>();
                    case 3: return std::vector<glm::vec3>();
                    case 4: return std::vector<glm::vec4>();
                }
        }
    }
    static inline VectorVariant<Mesh::attribute_t> generateAttributeDataset(Mesh::AttributeType type) {
        switch (type) {
            default:
            case Mesh::AttributeType::Integer: return std::vector<int>();
            case Mesh::AttributeType::IVec2: return std::vector<glm::ivec2>();
            case Mesh::AttributeType::IVec3: return std::vector<glm::ivec3>();
            case Mesh::AttributeType::IVec4: return std::vector<glm::ivec4>();

            case Mesh::AttributeType::Float: return std::vector<float>();
            case Mesh::AttributeType::Vec2: return std::vector<glm::vec2>();
            case Mesh::AttributeType::Vec3: return std::vector<glm::vec3>();
            case Mesh::AttributeType::Vec4: return std::vector<glm::vec4>();
        }
    }
    static inline std::string getPrimitiveTypeLabel(Mesh::Type type) {
        switch (type) {
            default: return "Undefined";
            case Mesh::Type::Triangles     : return "Triangles";
            case Mesh::Type::TriangleStrip : return "Triangle Strip";
            case Mesh::Type::TriangleFan   : return "Triangle Fan";
            case Mesh::Type::Lines         : return "Lines";
            case Mesh::Type::Points        : return "Points";
        }
    }

    std::unique_ptr<Mesh> mMesh;
    Port<Mesh*> mMeshOut = Port<Mesh*>(*this, IPort::Direction::Out, "MeshOut", "Mesh", [&]() { return mMesh.get(); });

    unsigned int mNumVertices = 1;
    unsigned int mNumIndices = 0;

    Mesh::Type mType = Mesh::Type::Triangles;

    std::vector<Attribute> mAttributes;
    std::vector<unsigned int> mIndices{};

    std::filesystem::path mFilepath;
    bool mCanWriteToFile = true;

    bool mShowAttributes = false;
    bool mShowAddAttribute = false;
};

