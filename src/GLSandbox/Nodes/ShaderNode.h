#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Shader.h"

#include "../Assets.h"
#include "../NodeClassifications.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class ShaderNode final : public Node {
public:
    ShaderNode();
    ~ShaderNode() final = default;

    [[nodiscard]] unsigned int getTypeID() const final {
        return (unsigned int)NodeType::Shader;
    }

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] inline const Shader& getShader() const {
        return *mShader;
    }
protected:
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> generateSerializedData() const final;
    void deserializeData(const std::string& dataID, std::ifstream& stream) final;

    void onDeserialize() final;

    void drawContents() override;
private:
    enum class ShaderFileState {
        Unloaded,
        ReadSuccess,
        ReadFailure,
    };

    struct ShaderPassData {
        ShaderPassData(std::string displayName, const std::vector<std::string>& validExtensions) :
        displayName(displayName), validExtensions(validExtensions) {}

        ShaderFileState state = ShaderFileState::Unloaded;
        std::filesystem::path filepath;
        std::string code;
        std::string displayName;
        const std::vector<std::string>& validExtensions;
    };

    typedef std::function<void()> update_files_callback;
    typedef std::function<std::vector<std::string>&()> get_files_callback;

    void drawShaderInput(ShaderPassData& data);

    void drawShaderStatus();

    void uploadShader();

    std::unique_ptr<Shader> mShader = std::make_unique<Shader>();
    Port<Shader*> mShaderOut = Port<Shader*>(*this, IPort::Direction::Out, "ShaderOut", "Shader", [&]() { return mShader.get(); });

    ShaderPassData mVertData = ShaderPassData("Vertex",   getValidVertexShaderFileExtensions());
    ShaderPassData mFragData = ShaderPassData("Fragment",  getValidFragmentShaderFileExtensions());
    ShaderPassData mTescData = ShaderPassData("Tess-Cont", getValidTessContShaderFileExtensions());
    ShaderPassData mTeseData = ShaderPassData("Tess-Eval", getValidTessEvalShaderFileExtensions());
    ShaderPassData mGeomData = ShaderPassData("Geometry",  getValidGeometryShaderFileExtensions());
};

