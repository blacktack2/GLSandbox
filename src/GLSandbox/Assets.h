#pragma once
#include <cstring>
#include <string>

enum class MeshFileExtension {
    MSH = 0,
    OBJ,

    Max,
    Undefined,
};

#ifdef _WIN32
static const std::string gDIR_DELIMITER = "\\";
#else
static const std::string gDIR_DELIMITER = "/";
#endif // _WIN32
static const std::string gSHADER_ASSET_DIR  = "Shaders"  + gDIR_DELIMITER;
static const std::string gMESH_ASSET_DIR    = "Meshes"   + gDIR_DELIMITER;
static const std::string gTEXTURE_ASSET_DIR = "Textures" + gDIR_DELIMITER;

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
