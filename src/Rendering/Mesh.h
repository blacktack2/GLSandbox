#pragma once
#include <glm/glm.hpp>

#include <fstream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

class Mesh {
public:
    typedef std::variant<
        int, glm::ivec2, glm::ivec3, glm::ivec4,
        float, glm::vec2, glm::vec3, glm::vec4
    > attribute_t;
    enum class AttributeType {
        Integer = 0,
        IVec2,
        IVec3,
        IVec4,

        Float,
        Vec2,
        Vec3,
        Vec4,

        Max,
        Undefined,
    };

    enum class ErrorState {
        INVALID,
        VALID,
    };

    enum class Type {
        Triangles = 0,
        TriangleStrip,
        TriangleFan,
        Lines,
        Points,

        Max,
    };

    Mesh();
    ~Mesh();

    void softClean();
    void hardClean();

    void setType(Type type);
    inline void setNumVertices(unsigned int numVertices) {
        mNumVertices = numVertices;
    }

    inline void setIndices(const std::vector<unsigned int>& indices) {
        mIndices = indices;
    }

    void draw() const;

    void draw(int instanceCount) const;

    void bind() const;

    static void unbind();

    /**
     *
     * @param data Actual vertex data to be uploaded. Caller is responsible for maintaining the lifecycle of the data.
     * @param attribSize Number of elements in the data type (e.g. vec3 is 3).
     * @param dataSize Size of the data type (e.g. vec3 is sizeof(vec3)).
     * @param debugName Label for printing debug information about this attribute.
     */
    void addAttribute(const void* data, int attribSize, size_t dataSize, const std::string& debugName);
    void bufferData();

    static void makeScreenQuad(Mesh& mesh);

    [[nodiscard]] inline ErrorState getState() const {
        return mErrorState;
    }

    [[nodiscard]] inline const std::string& getErrorMessage() {
        return mMessage;
    }
private:
    struct VertexAttribute {
        unsigned int vbo = 0;

        unsigned int index{};
        const void* data{};
        int attribSize{};
        size_t dataSize{};
        std::string debugName;
    };

    void uploadAttribute(VertexAttribute& attrib) const;

    void uploadIndices();

    unsigned int mArrayObject = 0;

    unsigned int mType = 0;
    unsigned int mNumVertices = 0;

    unsigned int mIndexVBO = 0;

    std::vector<unsigned int> mIndices;
    std::vector<VertexAttribute> mAttributes{};

    ErrorState mErrorState = ErrorState::INVALID;
    std::string mMessage;
};

