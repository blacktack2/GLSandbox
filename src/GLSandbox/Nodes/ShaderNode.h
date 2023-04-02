#pragma once
#include "../../NodeEditor/Node.h"

#include "../../Rendering/Shader.h"
#include "../NodeClassifications.h"

#include <memory>
#include <string>
#include <vector>

class ShaderNode final : public Node {
public:
    ShaderNode();
    ~ShaderNode() final = default;

    [[nodiscard]] unsigned int getTypeID() final {
        return (unsigned int)NodeType::Shader;
    }

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] inline const Shader& getShader() const {
        return *mShader;
    }

    static void findVertexFiles();
    static void findFragmentFiles();
    static void findTessContFiles();
    static void findTessEvalFiles();
    static void findGeometryFiles();
protected:
    void serializeContents(std::ofstream& streamOut) const final;
    void deserializeContents(std::ifstream& streamIn) final;

    void drawContents() override;
private:
    void drawShaderCombo(const std::string& label, int& index, const std::vector<std::string>& files);
    void drawShaderStatus();

    void updateShader();

    static void findFiles(std::vector<std::string>& files, const std::vector<std::string>& extensions);

    static inline const std::string& getShaderDir() {
        static const std::string cDirectory = "Shaders/";
        return cDirectory;
    }

    static inline std::vector<std::string>& getVertexFiles() {
        static std::vector<std::string> cFiles;
        return cFiles;
    }
    static inline std::vector<std::string>& getFragmentFiles() {
        static std::vector<std::string> cFiles;
        return cFiles;
    }
    static inline std::vector<std::string>& getTessContFiles() {
        static std::vector<std::string> cFiles;
        return cFiles;
    }
    static inline std::vector<std::string>& getTessEvalFiles() {
        static std::vector<std::string> cFiles;
        return cFiles;
    }
    static inline std::vector<std::string>& getGeometryFiles() {
        static std::vector<std::string> cFiles;
        return cFiles;
    }

    static inline const std::vector<std::string>& getValidVertexFileExtensions() {
        static const std::vector<std::string> cExtensions = {".glsl", ".vert", ".vs", ".glslv"};
        return cExtensions;
    }
    static inline const std::vector<std::string>& getValidFragmentFileExtensions() {
        static const std::vector<std::string> cExtensions = {".glsl", ".frag", ".fs", ".glslf"};
        return cExtensions;
    }
    static inline const std::vector<std::string>& getValidTessContFileExtensions() {
        static const std::vector<std::string> cExtensions = {".glsl", ".tesc", ".tcs", ".glsltc"};
        return cExtensions;
    }
    static inline const std::vector<std::string>& getValidTessEvalFileExtensions() {
        static const std::vector<std::string> cExtensions = {".glsl", ".tese", ".tes", ".glslte"};
        return cExtensions;
    }
    static inline const std::vector<std::string>& getValidGeometryFileExtensions() {
        static const std::vector<std::string> cExtensions = {".glsl", ".geom", ".gs", ".glslg"};
        return cExtensions;
    }

    std::unique_ptr<Shader> mShader = std::make_unique<Shader>();
    OutPort mShaderOutPort = OutPort(*this, "Shader");

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
};

