#include "Renderer.h"

Renderer::Renderer() {
    loadShader("Shaders/Skybox.vert", "Shaders/Skybox.frag");
    mMeshes.emplace_back(std::make_shared<Mesh>());
    Mesh::makeScreenQuad(*mMeshes[0]);
}

Renderer::~Renderer() = default;

void Renderer::update() {

}

void Renderer::draw() {
    mShaders[0]->bind();

    mMeshes[0]->bind();
    mMeshes[0]->draw();
    Mesh::unbind();

    Shader::unbind();
}

void Renderer::loadShader(const std::string& vert, const std::string& frag, const std::string& tesc, const std::string& tese, const std::string& geom) {
    mShaders.emplace_back(std::make_shared<Shader>(vert, frag, tesc, tese, geom));
}
