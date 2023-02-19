#include "Mesh.h"

#include <glm/vec2.hpp>

Mesh::Mesh() {
    glGenVertexArrays(1, &mArrayObject);
    hardClean();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &mArrayObject);
    softClean();
}

void Mesh::softClean() {
    for (VertexAttribute& attrib : mAttributes)
        glDeleteBuffers(1, &attrib.vbo);
}

void Mesh::hardClean() {
    softClean();

    mType = GL_TRIANGLES;
    mNumVertices = 0;

    mIndexVBO = 0;

    mUseIndexBuffer = false;

    mIndices.clear();
    mAttributes.clear();
}

void Mesh::addAttribute(void* data, GLint attribSize, size_t dataSize, const std::string& debugName) {
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

    mesh.setType(GL_TRIANGLE_STRIP);
    mesh.setNumVertices(4);
    mesh.addAttribute(positions, 2, sizeof(glm::vec2), "Position");
    mesh.bind();
    mesh.bufferData();
    unbind();
}

void Mesh::bufferData() {
    for (VertexAttribute& attrib : mAttributes)
        uploadAttribute(attrib);
    if (!mIndices.empty())
        uploadIndices();
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
}