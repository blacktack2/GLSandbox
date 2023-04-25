#include "Mesh.h"

#include <glad/glad.h>

Mesh::Mesh() {
    glGenVertexArrays(1, &mArrayObject);
    hardClean();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &mArrayObject);
    softClean();
}

void Mesh::softClean() {
    for (VertexAttribute& attrib : mAttributes) {
        glDeleteBuffers(1, &attrib.vbo);
        attrib.vbo = 0;
    }
    mErrorState = ErrorState::INVALID;
}

void Mesh::hardClean() {
    softClean();

    mType = GL_TRIANGLES;
    mNumVertices = 0;

    mIndexVBO = 0;

    mIndices.clear();
    mAttributes.clear();
}

void Mesh::draw() const {
    if (mIndices.empty())
        glDrawArrays(mType, 0, mNumVertices);
    else
        glDrawElements(mType, mIndices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::draw(int instanceCount) const {
    if (mIndices.empty())
        glDrawArraysInstanced(mType, 0, mNumVertices, instanceCount);
    else
        glDrawElementsInstanced(mType, mIndices.size(), GL_UNSIGNED_INT, nullptr, instanceCount);
}

void Mesh::bind() const {
    glBindVertexArray(mArrayObject);
    if (!mIndices.empty())
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
}

void Mesh::unbind() {
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::addAttribute(const void* data, GLint attribSize, size_t dataSize, const std::string& debugName) {
    VertexAttribute& attribute = mAttributes.emplace_back();
    attribute.index = mAttributes.size() - 1;
    attribute.data = data;
    attribute.attribSize = attribSize;
    attribute.dataSize = dataSize;
    attribute.debugName = debugName;
}

void Mesh::makeScreenQuad(Mesh& mesh) {
    mesh.hardClean();

    glm::vec2 positions[] = {
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        {-1.0f,  1.0f},
        { 1.0f,  1.0f},
    };

    mesh.setType(Type::TriangleStrip);
    mesh.setNumVertices(4);
    mesh.addAttribute(positions, 2, sizeof(glm::vec2), "Position");
    mesh.bind();
    mesh.bufferData();
    unbind();
}

void Mesh::bufferData() {
    bind();
    for (VertexAttribute& attrib : mAttributes)
        uploadAttribute(attrib);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (!mIndices.empty())
        uploadIndices();

    mErrorState = ErrorState::VALID;
}

void Mesh::uploadAttribute(Mesh::VertexAttribute& attrib) const {
    glGenBuffers(1, &attrib.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, attrib.vbo);

    glBufferData(GL_ARRAY_BUFFER, mNumVertices * attrib.dataSize, attrib.data, GL_STATIC_DRAW);
    glVertexAttribPointer(attrib.index, attrib.attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attrib.index);
}

void Mesh::uploadIndices() {
    glGenBuffers(1, &mIndexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glObjectLabel(GL_BUFFER, mIndexVBO, -1, "Indices");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::setType(Mesh::Type type) {
    GLuint gltype;
    switch (type) {
        default:
        case Type::Triangles     : gltype = GL_TRIANGLES     ; break;
        case Type::TriangleStrip : gltype = GL_TRIANGLE_STRIP; break;
        case Type::TriangleFan   : gltype = GL_TRIANGLE_FAN  ; break;
        case Type::Lines         : gltype = GL_LINES         ; break;
        case Type::Points        : gltype = GL_POINTS        ; break;
    }
    mType = gltype;
}
