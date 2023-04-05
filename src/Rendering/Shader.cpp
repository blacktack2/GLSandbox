#include "Shader.h"

#include <fstream>
#include <regex>
#include <sstream>

Shader::Shader(const std::string& vertShader, const std::string& fragShader,
               const std::string& tescShader, const std::string& teseShader, const std::string& geomShader) :
mProgramID(glCreateProgram()) {
    mShaderPasses.emplace_back(vertShader, GL_VERTEX_SHADER);
    mShaderPasses.emplace_back(fragShader, GL_FRAGMENT_SHADER);
    if (!tescShader.empty())
        mShaderPasses.emplace_back(tescShader, GL_TESS_CONTROL_SHADER);
    if (!teseShader.empty())
        mShaderPasses.emplace_back(teseShader, GL_TESS_EVALUATION_SHADER);
    if (!geomShader.empty())
        mShaderPasses.emplace_back(geomShader, GL_GEOMETRY_SHADER);

    if (loadShaders())
        mState = ErrorState::VALID;
}

Shader::~Shader() {
    glDeleteProgram(mProgramID);
}

void Shader::setUniformAny(const std::string& name, const std::any& value) {
    if (value.type() == typeid(float))
        setUniform(name, std::any_cast<float>(value));
    else if (value.type() == typeid(glm::vec2))
        setUniform(name, std::any_cast<glm::vec2>(value));
    else if (value.type() == typeid(glm::vec3))
        setUniform(name, std::any_cast<glm::vec3>(value));
    else if (value.type() == typeid(glm::vec4))
        setUniform(name, std::any_cast<glm::vec4>(value));
    else if (value.type() == typeid(int))
        setUniform(name, std::any_cast<int>(value));
    else if (value.type() == typeid(glm::ivec2))
        setUniform(name, std::any_cast<glm::ivec2>(value));
    else if (value.type() == typeid(glm::ivec3))
        setUniform(name, std::any_cast<glm::ivec3>(value));
    else if (value.type() == typeid(glm::ivec4))
        setUniform(name, std::any_cast<glm::ivec4>(value));
    else if (value.type() == typeid(glm::mat2))
        setUniform(name, std::any_cast<glm::mat2>(value));
    else if (value.type() == typeid(glm::mat3))
        setUniform(name, std::any_cast<glm::mat3>(value));
    else if (value.type() == typeid(glm::mat4))
        setUniform(name, std::any_cast<glm::mat4>(value));
    else
        assert(false);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-make-member-function-const"
void Shader::setUniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, glm::vec2 value) {
    glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, glm::vec3 value) {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, glm::vec4 value) {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, glm::ivec2 value) {
    glUniform2iv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, glm::ivec3 value) {
    glUniform3iv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, glm::ivec4 value) {
    glUniform4iv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, glm::mat2 value) {
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const std::string& name, glm::mat3 value) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const std::string& name, glm::mat4 value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
#pragma clang diagnostic pop

std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::type_index>>>> Shader::getUniforms() {
    static const std::regex cUNIFORM_REGEX("uniform +([a-zA-Z0-9_]+) +([a-zA-Z0-9_]+)( *= *(.*) *)?;");

    std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::type_index>>>> uniforms;

    for (const ShaderPass& pass : mShaderPasses) {
        std::string passName;
        switch (pass.type) {
            case GL_VERTEX_SHADER          : passName = "Vertex"   ; break;
            case GL_FRAGMENT_SHADER        : passName = "Fragment" ; break;
            case GL_TESS_CONTROL_SHADER    : passName = "Tess-Cont"; break;
            case GL_TESS_EVALUATION_SHADER : passName = "Tess-Eval"; break;
            case GL_GEOMETRY_SHADER        : passName = "Geometry" ; break;
            default                        : passName = "<error>"  ; break;
        }
        std::vector<std::pair<std::string, std::type_index>> passUniforms;

        std::string code = pass.code;
        code.erase(std::remove(code.begin(), code.end(), '\n'), code.end());

        for (std::sregex_iterator i = std::sregex_iterator(code.begin(), code.end(), cUNIFORM_REGEX);
                                  i != std::sregex_iterator(); ++i) {
            std::smatch match = *i;
            std::string type = match[1];
            std::string name = match[2];
            std::string defaultValue = match[4];

            std::type_index actualType = getUniformTypeMap().find(type)->second;
            passUniforms.emplace_back(name, actualType);
        }
        uniforms.emplace_back(passName, std::move(passUniforms));
    }

    return uniforms;
}

bool Shader::loadShaders() {
    for (ShaderPass& pass : mShaderPasses) {
        if (!readShader(pass.filename, pass.code))
            return false;
        if (!compileShader(pass.code, pass.type))
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
