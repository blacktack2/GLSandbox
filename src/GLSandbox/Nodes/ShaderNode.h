#pragma once
#include "../../NodeEditor/Node.h"

#include "../../NodeEditor/Ports.h"

#include "../../Rendering/Shader.h"
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
    typedef std::function<void()> update_files_callback;
    typedef std::function<std::vector<std::string>&()> get_files_callback;

    void drawShaderStatus();

    void uploadShader();

    std::unique_ptr<Shader> mShader = std::make_unique<Shader>();
    Port<Shader*> mShaderOut = Port<Shader*>(*this, IPort::Direction::Out, "ShaderOut", "Shader", [&]() { return mShader.get(); });

    std::filesystem::path mVertFilepath;
    std::filesystem::path mFragFilepath;
    std::filesystem::path mTescFilepath;
    std::filesystem::path mTeseFilepath;
    std::filesystem::path mGeomFilepath;
};

