#include "Shader.h"

#include <fstream>
#include <sstream>

Shader::Shader() {

}

Shader::Shader(const std::string& vertShader, const std::string& fragShader,
               const std::string& tescShader, const std::string& teseShader, const std::string& geomShader) :
mProgramID(glCreateProgram()) {
    std::vector<ShaderConfig> shaders;

    shaders.emplace_back(vertShader, GL_VERTEX_SHADER);
    shaders.emplace_back(fragShader, GL_FRAGMENT_SHADER);
    if (!tescShader.empty())
        shaders.emplace_back(tescShader, GL_TESS_CONTROL_SHADER);
    if (!teseShader.empty())
        shaders.emplace_back(teseShader, GL_TESS_EVALUATION_SHADER);
    if (!geomShader.empty())
        shaders.emplace_back(geomShader, GL_GEOMETRY_SHADER);

    if (loadShaders(shaders))
        mState = ErrorState::VALID;
}

Shader::~Shader() {
    glDeleteProgram(mProgramID);
}

bool Shader::loadShaders(std::vector<ShaderConfig>& shaders) {
    for (auto& data : shaders) {
        std::string code;
        if (!readShader(data.filename, code))
            return false;
        if (!compileShader(code, data.type))
            return false;
    }
    return linkProgram();
}

bool Shader::readShader(const std::string& filename, std::string& code) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        mState = ErrorState::FILE_READ;
        mMessage = std::string("Failed to open shader file: ").append(filename);
        return false;
    }

    std::stringstream sstream;
    sstream << file.rdbuf();
    if (!file || !sstream) {
        mState = ErrorState::FILE_READ;
        mMessage = std::string("Error reading shader file: ").append(filename);
        return false;
    }
    code = sstream.str();
    return true;
}

bool Shader::compileShader(const std::string& code, GLenum type) {
    GLuint shader = glCreateShader(type);

    const GLchar* data = code.c_str();
    glShaderSource(shader, 1, &data, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        mState = ErrorState::OGL_COMPILE;
        mMessage = std::string(512, '\0');
        glGetShaderInfoLog(shader, 512, nullptr, mMessage.data());
        return false;
    }

    glAttachShader(mProgramID, shader);
    glDeleteShader(shader);
    return true;
}

bool Shader::linkProgram() {
    glLinkProgram(mProgramID);

    GLint success;
    glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        mState = ErrorState::OGL_LINK;
        mMessage = std::string(512, '\0');
        glGetProgramInfoLog(mProgramID, 512, nullptr, mMessage.data());
        return false;
    }
    return true;
}
