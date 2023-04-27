#pragma once
#include "../Utils/VariantUtils.h"
#include "../Utils/GLMUtils.h"

#include <glm/glm.hpp>

#include <any>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

class Texture;

class Shader {
public:
    typedef std::variant<
        float, glm::vec2,  glm::vec3,  glm::vec4,
        int,   glm::ivec2, glm::ivec3, glm::ivec4,
               glm::mat2,  glm::mat3,  glm::mat4,
        Texture*
    > uniform_t;

    enum class ErrorState {
        INVALID,
        VALID,
        FILE_READ,
        OGL_COMPILE,
        OGL_LINK,
    };

    struct Uniform {
        std::string name;
        uniform_t value;
    };
    struct UniformSet {
        std::string name;
        std::vector<Uniform> uniforms;
    };

    Shader() = default;
    Shader(const std::string& vertCode, const std::string& fragCode,
           const std::string& tescCode = "", const std::string& teseCode = "", const std::string& geomCode = "");
    ~Shader();

    void setUniformAny(const std::string& name, const std::any& value);

    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, glm::vec2 value);
    void setUniform(const std::string& name, glm::vec3 value);
    void setUniform(const std::string& name, glm::vec4 value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, glm::ivec2 value);
    void setUniform(const std::string& name, glm::ivec3 value);
    void setUniform(const std::string& name, glm::ivec4 value);
    void setUniform(const std::string& name, glm::mat2 value);
    void setUniform(const std::string& name, glm::mat3 value);
    void setUniform(const std::string& name, glm::mat4 value);

    [[nodiscard]] int getSamplerLocation(const std::string& name) const {
        return std::find(mSamplers.begin(), mSamplers.end(), name) - mSamplers.begin();
    }

    [[nodiscard]] int getUniformLocation(const std::string& name) const;

    void bind() const;

    static void unbind();

    [[nodiscard]] inline ErrorState getState() const {
        return mState;
    }

    [[nodiscard]] inline std::string getErrorMessage() const {
        return mMessage;
    }

    /**
     * @brief Parse all uniforms from shader code into a list of valid uniforms.
     * @brief All passes are assumed to be valid.
     */
    std::vector<UniformSet> getUniforms();
private:
    struct ShaderPass {
        ShaderPass(std::string code, unsigned int type) : code(std::move(code)), type(type) {}
        unsigned int type;
        std::string code;
    };
    bool loadShaders();
    bool compileShader(const std::string& code, unsigned int type);
    bool linkProgram();

    /**
     * @brief Find all uniform variables in code and use them to populate uniforms.
     * @param code glsl format shader code to parse.
     * @param uniforms Vector of uniforms to populate with uniform data.
     */
    void parseUniforms(const std::string& code, std::vector<Uniform>& uniforms);
    /**
     * @param type glsl format data type.
     * @param defaultValue glsl format variable instantiation (if present).
     * @return Variant matching the primitive or glm equivalent type specified in type.
     */
    static uniform_t generateUniform(const std::string& type, const std::string& defaultValue = "");

    std::vector<ShaderPass> mShaderPasses;

    unsigned int mProgramID = 0;

    std::vector<std::string> mSamplers{};

    ErrorState mState = ErrorState::INVALID;
    std::string mMessage = "Not Initialized";
};
