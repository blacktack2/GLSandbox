#pragma once
#include <cstring>

enum class MeshFileExtension {
    MSH = 0,
    OBJ,

    Max,
    Undefined,
};

static const char* gSHADER_ASSET_DIR = "Shaders/";
static const char* gMESH_ASSET_DIR = "Meshes/";
static const char* gTEXTURE_ASSET_DIR = "Textures/";

static constexpr MeshFileExtension gMESH_DEFAULT_EXTENSION = MeshFileExtension::MSH;

static inline constexpr const char* getFileExtension(MeshFileExtension extension) {
    switch (extension) {
        case MeshFileExtension::MSH: return ".msh";
        case MeshFileExtension::OBJ: return ".obj";
        default: return "UNDEFINED";
    }
}
static inline MeshFileExtension parseFileExtension(const char* extension) {
    for (unsigned int i = 0; i < (unsigned int)MeshFileExtension::Max; i++) {
        const char* fileExt = getFileExtension((MeshFileExtension)i);
        if (std::strcmp(fileExt, extension) == 0)
            return (MeshFileExtension)i;
    }
    return MeshFileExtension::Undefined;
}
