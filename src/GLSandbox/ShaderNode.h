#pragma once
#include "../NodeEditor/Node.h"
#include "../Rendering/Shader.h"

#include <string>
#include <vector>

class ShaderNode : public Node {
public:
    ShaderNode();

    static void findVertexFiles();
    static void findFragmentFiles();
    static void findTessContFiles();
    static void findTessEvalFiles();
    static void findGeometryFiles();
protected:
    void drawContents() override;
    void runContents() override;
private:
    void drawShaderCombo(const std::string& label, int& index, const std::vector<std::string>& files);
    void drawShaderStatus();

    void updateShader();

    static void findFiles(std::vector<std::string>& files, const std::vector<std::string>& extensions);

    Shader mShader;

    int vert = -1;
    int frag = -1;
    int tesc = -1;
    int tese = -1;
    int geom = -1;

    bool vertLoaded = false;
    bool fragLoaded = false;
    bool tescLoaded = false;
    bool teseLoaded = false;
    bool geomLoaded = false;

    static std::vector<std::string> sVertexFiles;
    static std::vector<std::string> sFragmentFiles;
    static std::vector<std::string> sTessContFiles;
    static std::vector<std::string> sTessEvalFiles;
    static std::vector<std::string> sGeometryFiles;

    static const std::string SHADER_DIR;

    static const std::vector<std::string> VALID_VERTEX;
    static const std::vector<std::string> VALID_FRAGMENT;
    static const std::vector<std::string> VALID_TESS_CONT;
    static const std::vector<std::string> VALID_TESS_EVAL;
    static const std::vector<std::string> VALID_GEOMETRY;
};

