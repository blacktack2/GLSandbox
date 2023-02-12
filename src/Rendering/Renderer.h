#pragma once
#include "Mesh.h"
#include "Shader.h"

#include <memory>
#include <string>
#include <vector>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void update();
    void draw();

    void loadShader(const std::string& vert, const std::string& frag, const std::string& tesc = "", const std::string& tese = "", const std::string& geom = "");
private:
    std::vector<std::shared_ptr<Shader>> mShaders;
    std::vector<std::shared_ptr<Mesh>> mMeshes;
};
