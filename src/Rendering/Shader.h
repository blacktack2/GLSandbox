#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>

#include <any>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

class Shader {
public:
    enum class ErrorState {
        INVALID,
        VALID,
        FILE_READ,
        OGL_COMPILE,
        OGL_LINK,
    };

    Shader() = default;
    Shader(const std::string& vertShader, const std::string& fragShader,
           const std::string& tescShader = "", const std::string& teseShader = "", const std::string& geomShader = "");
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

    inline int getUniformLocation(const std::string& name) const {
        return glGetUniformLocation(mProgramID, name.c_str());
    }

    inline void bind() const {
        glUseProgram(mProgramID);
    }

    static inline void unbind() {
        glUseProgram(0);
    }

    [[nodiscard]] inline ErrorState getState() const {
        return mState;
    }

    [[nodiscard]] inline std::string getErrorMessage() const {
        return mMessage;
    }

    std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::type_index>>>> getUniforms();

    static inline const std::unordered_map<std::string, std::type_index>& getUniformTypeMap() {
        static const std::unordered_map<std::string, std::type_index> UNIFORM_TYPE_MAP = {
            {"float", typeid(float)},
            {"vec2", typeid(glm::vec2)},
            {"vec3", typeid(glm::vec3)},
            {"vec4", typeid(glm::vec4)},
            {"int", typeid(int)},
            {"ivec2", typeid(glm::ivec2)},
            {"ivec3", typeid(glm::ivec3)},
            {"ivec4", typeid(glm::ivec4)},
            {"mat2", typeid(glm::mat2)},
            {"mat3", typeid(glm::mat3)},
            {"mat4", typeid(glm::mat4)},
        };
        return UNIFORM_TYPE_MAP;
    }
private:
    struct ShaderPass {
        ShaderPass(std::string filename, GLenum type) : filename(std::move(filename)), type(type) {}
        std::string filename;
        GLenum type;
        std::string code;
    };
    bool loadShaders();
    bool readShader(const std::string& filename, std::string& code);
    bool compileShader(const std::string& code, GLenum type);
    bool linkProgram();

    std::vector<ShaderPass> mShaderPasses;

    GLuint mProgramID = 0;

    ErrorState mState = ErrorState::INVALID;
    std::string mMessage = "Not Initialized";
};
