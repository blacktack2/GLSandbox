#pragma once
#include <glad/glad.h>

#include <memory>
#include <string>
#include <vector>

class Mesh {
public:
    Mesh();
    ~Mesh();

    void softClean();
    void hardClean();

    inline void setType(GLuint type) {
        mType = type;
    }
    inline void setNumVertices(GLuint numVertices) {
        mNumVertices = numVertices;
    }

    inline void setIndices(const std::vector<GLuint>& indices) {
        mIndices = indices;
    }

    inline void useIndexBuffer(bool use) {
        mUseIndexBuffer = use;
    }

    inline void draw() const {
        if (mUseIndexBuffer)
            glDrawElements(mType, mIndices.size(), GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mType, 0, mNumVertices);
    }

    inline void draw(GLsizei instanceCount) const {
        if (mUseIndexBuffer)
            glDrawElementsInstanced(mType, mIndices.size(), GL_UNSIGNED_INT, nullptr, instanceCount);
        else
            glDrawArraysInstanced(mType, 0, mNumVertices, instanceCount);
    }

    inline void bind() const {
        glBindVertexArray(mArrayObject);
    }

    static inline void unbind() {
        glBindVertexArray(0);
    }

    /**
     *
     * @param data Actual vertex data to be uploaded.
     * @param attribSize Number of elements in the data type (e.g. vec3 is 3).
     * @param dataSize Size of the data type (e.g. vec3 is sizeof(vec3)).
     * @param debugName Label for printing debug information about this attribute.
     */
    void addAttribute(void* data, GLint attribSize, size_t dataSize, const std::string& debugName);
    void bufferData();

    static void makeScreenQuad(Mesh& mesh);
private:
    struct VertexAttribute {
        GLuint vbo = 0;

        GLuint index{};
        void* data{};
        GLint attribSize{};
        size_t dataSize{};
        std::string debugName;
    };

    void uploadAttribute(VertexAttribute& attrib) const;

    void uploadIndices();

    GLuint mArrayObject = 0;

    GLuint mType;
    GLuint mNumVertices;

    GLuint mIndexVBO;

    bool mUseIndexBuffer = false;

    std::vector<GLuint> mIndices;
    std::vector<VertexAttribute> mAttributes{};
};

