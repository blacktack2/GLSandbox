#pragma once
#include "../../NodeEditor/Node.h"
#include "../../Rendering/Mesh.h"

#include "../NodeClassifications.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <any>
#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class MeshNode final : public Node {
public:
    MeshNode();
    ~MeshNode() final = default;

    void clearAttributes();

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Mesh;
    }

    [[nodiscard]] inline const Mesh& getMesh() const {
        return mMesh;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void drawContents() override;
private:
    class IAttribute {
    public:
        IAttribute() = default;
        virtual ~IAttribute() = default;

        virtual void serialize(std::ofstream& stream) const = 0;
        virtual void deserialize(std::ifstream& stream) = 0;

        [[nodiscard]] virtual unsigned int getID() const = 0;

        [[nodiscard]] virtual void* getData() const = 0;
        [[nodiscard]] virtual void* getDataAt(size_t index) const = 0;
        virtual void resizeData(size_t size) = 0;

        [[nodiscard]] virtual const std::string& getName() const = 0;
        virtual void setName(const std::string& name) = 0;

        [[nodiscard]] virtual bool isShown() const = 0;
        virtual void setShown(bool show) = 0;

        [[nodiscard]] virtual bool isMarkedDelete() const = 0;
        virtual void markForDeletion() = 0;

        [[nodiscard]] virtual unsigned int getNumComponents() const = 0;
    };

    template<typename T>
    class Attribute final : public IAttribute {
    public:
        Attribute() {
            static unsigned int sIdCounter = 0;
            mID = sIdCounter++;
        }
        ~Attribute() final = default;

        void serialize(std::ofstream& stream) const final {
            stream << (mName.empty() ? "attribute" : mName);

            if constexpr (std::disjunction_v<std::is_same<T, int>, std::is_same<T, float>>)
                stream << ' ' << 1u;
            else
                stream << ' ' << (unsigned int)T::length();

            if constexpr (std::disjunction_v<std::is_same<T, int>, std::is_same<T, glm::ivec2>,
                    std::is_same<T, glm::ivec3>, std::is_same<T, glm::ivec4>>)
                stream << ' ' << 'i';
            else
                stream << ' ' << 'f';

            stream << '\n';

            for (const auto& val : mData) {
                if constexpr (std::disjunction_v<std::is_same<T, int>, std::is_same<T, float>>) {
                    stream << val;
                } else {
                    stream << val[0];
                    for (size_t i = 1; i < T::length(); i++)
                        stream << ' ' << val[i];
                }

                stream << '\n';
            }
        }
        void deserialize(std::ifstream& stream) final {
            for (auto& val : mData) {
                if constexpr (std::disjunction_v<std::is_same<T, int>, std::is_same<T, float>>)
                    stream >> val;
                else
                    for (size_t i = 0; i < T::length(); i++)
                        stream >> val[i];
            }
        }

        [[nodiscard]] unsigned int getID() const final {
            return mID;
        }

        [[nodiscard]] void* getData() const final {
            return (void*)mData.data();
        }
        [[nodiscard]] void* getDataAt(size_t index) const final {
            return (void*)&mData[index];
        }
        void resizeData(size_t size) final {
            mData.resize(size);
        }
        void setData(const std::vector<T> data) {
            mData = data;
        }

        [[nodiscard]] const std::string& getName() const final {
            return mName;
        }
        void setName(const std::string& name) final {
            mName = name;
        }

        [[nodiscard]] bool isShown() const final {
            return mShow;
        }
        void setShown(bool show) final {
            mShow = show;
        }

        [[nodiscard]] bool isMarkedDelete() const final {
            return mRemove;
        }
        void markForDeletion() final {
            mRemove = true;
        }

        [[nodiscard]] unsigned int getNumComponents() const final {
            if constexpr (std::disjunction_v<std::is_same<T, int>, std::is_same<T, float>>) {
                return 1;
            } else {
                return T::length();
            }
        }
    private:
        unsigned int mID;
        std::string mName;
        std::vector<T> mData{};
        bool mShow = false;
        bool mRemove = false;
    };

    template<typename T>
    using draw_attribute_input_callback = std::function<void(const std::string&, T&)>;
    typedef std::function<std::unique_ptr<IAttribute>()> create_attribute_callback;

    void loadFromStreamOBJ(std::ifstream& stream);
    void loadFromStreamMSH(std::ifstream& stream);
    void writeToStreamMSH(std::ofstream& stream) const;

    std::string generateFilename() const;

    void uploadMesh();

    void drawGlobalParameters();

    void drawAttributes(std::vector<std::unique_ptr<IAttribute>>& attributes, std::type_index type);
    void drawAttribute(IAttribute& attribute, std::type_index type);

    void drawAddAttributePopup();
    void drawAttributeSelection(std::vector<std::unique_ptr<IAttribute>>& attrVec, std::type_index type);

    void drawUploadButton();

    void drawMeshStatus();

    std::string getNodeID();
    std::string getAttributeID(std::type_index type);

    void resizeAttributes();

    Mesh mMesh;
    OutPort mMeshOutPort = OutPort(*this, "MeshOut", "Mesh", [&]() { return &mMesh; });

    unsigned int mNumVertices = 1;
    unsigned int mNumIndices = 0;

    Mesh::Type mType = Mesh::Type::Triangles;

    std::unordered_map<std::type_index, std::vector<std::unique_ptr<IAttribute>>> mAttributes;
    std::map<std::type_index, create_attribute_callback> mAttributeFactories = {
        {typeid(int),        []() { return std::make_unique<Attribute<int>>();        }},
        {typeid(glm::ivec2), []() { return std::make_unique<Attribute<glm::ivec2>>(); }},
        {typeid(glm::ivec3), []() { return std::make_unique<Attribute<glm::ivec3>>(); }},
        {typeid(glm::ivec4), []() { return std::make_unique<Attribute<glm::ivec4>>(); }},
        {typeid(float),      []() { return std::make_unique<Attribute<float>>();      }},
        {typeid(glm::vec2),  []() { return std::make_unique<Attribute<glm::vec2>>();  }},
        {typeid(glm::vec3),  []() { return std::make_unique<Attribute<glm::vec3>>();  }},
        {typeid(glm::vec4),  []() { return std::make_unique<Attribute<glm::vec4>>();  }},
    };
    std::vector<GLuint> mIndices{};

    // Matches Mesh::Type
    const std::vector<std::string> mTypes {
        "Triangles",
        "Triangle Strip",
        "Triangle Fan",
        "Lines",
        "Points",
    };

    std::string mFilename;
};

