#pragma once
#include <filesystem>
#include <string>
#include <vector>

[[nodiscard]] static std::filesystem::path getGraphAssetDirectory() {
    static const std::filesystem::path cDIRECTORY = "Graphs";
    return cDIRECTORY;
}
[[nodiscard]] static std::filesystem::path getShaderAssetDirectory() {
    static const std::filesystem::path cDIRECTORY = "Shaders";
    return cDIRECTORY;
}
[[nodiscard]] static std::filesystem::path getMeshAssetDirectory() {
    static const std::filesystem::path cDIRECTORY = "Meshes";
    return cDIRECTORY;
}
[[nodiscard]] static std::filesystem::path getTextureAssetDirectory() {
    static const std::filesystem::path cDIRECTORY = "Textures";
    return cDIRECTORY;
}

[[nodiscard]] static const std::vector<std::string>& getValidGraphFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "graph",
    };
    return cEXTENSIONS;
}

[[nodiscard]] static const std::vector<std::string>& getValidShaderFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "glsl,vert,vs,glslv,frag,fs,glslf,tesc,tcs,glsltc,tese,tes,glslte,geom,gs,glslg)",
        "glsl",
        "vert", "vs", "glslv",
        "frag", "fs", "glslf",
        "tesc", "tcs", "glsltc",
        "tese", "tes", "glslte",
        "geom", "gs", "glslg",
    };
    return cEXTENSIONS;
}
static inline const std::vector<std::string>& getValidVertexShaderFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "glsl,vert,vs,glslv",
        "glsl", "vert", "vs", "glslv",
    };
    return cEXTENSIONS;
}
static inline const std::vector<std::string>& getValidFragmentShaderFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "glsl,frag,fs,glslf",
        "glsl", "frag", "fs", "glslf",
    };
    return cEXTENSIONS;
}
static inline const std::vector<std::string>& getValidTessContShaderFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "glsl,tesc,tcs,glsltc",
        "glsl", "tesc", "tcs", "glsltc",
    };
    return cEXTENSIONS;
}
static inline const std::vector<std::string>& getValidTessEvalShaderFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "glsl,tese,tes,glslte",
        "glsl", "tese", "tes", "glslte",
    };
    return cEXTENSIONS;
}
static inline const std::vector<std::string>& getValidGeometryShaderFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "glsl,geom,gs,glslg",
        "glsl", "geom", "gs", "glslg",
        };
    return cEXTENSIONS;
}

[[nodiscard]] static const std::vector<std::string>& getValidMeshFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "msh,obj",
        "msh", "obj",
    };
    return cEXTENSIONS;
}

[[nodiscard]] static const std::vector<std::string>& getValidTextureFileExtensions() {
    static const std::vector<std::string> cEXTENSIONS = {
        "png,PNG,jpg,JPG,jpeg,JPEG",
        "png", "PNG", "jpg", "JPG", "jpeg", "JPEG",
    };
    return cEXTENSIONS;
}

[[nodiscard]] static const std::string& getGraphDefaultExtension() {
    static const std::string cEXTENSION = getValidGraphFileExtensions()[0];
    return cEXTENSION;
}
[[nodiscard]] static const std::string& getMeshDefaultExtension() {
    static const std::string cEXTENSION = getValidMeshFileExtensions()[1];
    return cEXTENSION;
}
