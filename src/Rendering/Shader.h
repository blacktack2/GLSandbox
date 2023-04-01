#pragma once
#include <glad/glad.h>

#include <string>
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
private:
    struct ShaderConfig {
        ShaderConfig(const std::string& filename, GLenum type) : filename(filename), type(type) {}
        const std::string& filename;
        const GLenum type;
    };
    bool loadShaders(std::vector<ShaderConfig>& shaders);
    bool readShader(const std::string& filename, std::string& code);
    bool compileShader(const std::string& code, GLenum type);
    bool linkProgram();

    GLuint mProgramID = 0;

    ErrorState mState = ErrorState::INVALID;
    std::string mMessage = "Default Message";
};
