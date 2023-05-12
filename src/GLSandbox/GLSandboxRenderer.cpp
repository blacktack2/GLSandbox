#include "GLSandboxRenderer.h"

#include "../Utils/SerializationUtils.h"

#include <glad/glad.h>

#include <imgui.h>

#include <chrono>
#include <ctime>

std::vector<std::string> GLSandboxRenderer::sDebugMessages{};
bool                     GLSandboxRenderer::sErrorFlag = false;

GLSandboxRenderer::GLSandboxRenderer() : Renderer() {
    mQuad = std::make_unique<Mesh>();
    Mesh::makeScreenQuad(*mQuad);

    std::string defaultVertCode;
    SerializationUtils::readFile("Shaders/simple-quad.vert", defaultVertCode);
    std::string defaultFragCode;
    SerializationUtils::readFile("Shaders/simple-quad.frag", defaultFragCode);
    mDefaultShader = std::make_unique<Shader>(defaultVertCode, defaultFragCode);

    std::string profileVertCode;
    SerializationUtils::readFile("Shaders/profile.vert", profileVertCode);
    std::string profileFragCode;
    SerializationUtils::readFile("Shaders/profile.frag", profileFragCode);
    mProfileShader = std::make_unique<Shader>(profileVertCode, profileFragCode);

    resetPipeline();
}

void GLSandboxRenderer::update() {
    if (sErrorFlag)
        resetPipeline();
}

void GLSandboxRenderer::drawDebug() {
    ImGui::Begin("Debug");

    ImGui::BeginChild("DebugData", ImVec2(200.0f, 0.0f), true);

    float deltaTime = mTimer.tick().count();
    ImGui::Text("%.2f fps | %.2f ms", 1.0f / deltaTime, 1000.0f * deltaTime);

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("DebugMessages", ImVec2(0.0f, 0.0f), true);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));

    for (const std::string& message : sDebugMessages)
        ImGui::TextWrapped("%s", message.c_str());

    ImGui::PopStyleColor();

    ImGui::EndChild();

    ImGui::End();
}

void GLSandboxRenderer::resetPipeline() {
    clearPipeline(true);
    appendPipeline([&]() {
        mDefaultShader->bind();
        mQuad->bind();
        mQuad->draw();
    });
}

void GLSandboxRenderer::loadAnalysisPipeline(unsigned int bulk, std::size_t iterations) {
    mProfileShader->bind();
    mProfileShader->setUniform("bulk", (int)bulk);
    mProfileShader->unbind();

    clearPipeline(false);
    appendPipeline([&, iterations]() {
        for (std::size_t i = 0; i < iterations; i++) {
            mProfileShader->bind();
            mQuad->bind();
            mQuad->draw();
        }
    });
}

void GLSandboxRenderer::debugOutput(unsigned int source, unsigned int type, unsigned int id, unsigned int severity,
                                    int length, const char* message, const void* userParam) {
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string debugMessage = std::string("Error at - ").append(std::ctime(&time)).append("\nSource: ");
    switch (source) {
        case GL_DEBUG_SOURCE_API             : debugMessage.append("API")            ; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM   : debugMessage.append("Window System")  ; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER : debugMessage.append("Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY     : debugMessage.append("Third Party")    ; break;
        case GL_DEBUG_SOURCE_APPLICATION     : debugMessage.append("Application")    ; break;
        case GL_DEBUG_SOURCE_OTHER           : debugMessage.append("Other")          ; break;
        default                              : debugMessage.append("Unknown")        ; break;
    }
    debugMessage.append(" | Direction: ");
    switch (type) {
        case GL_DEBUG_TYPE_ERROR               : debugMessage.append("Error")               ; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR : debugMessage.append("Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  : debugMessage.append("Undefined Behaviour") ; break;
        case GL_DEBUG_TYPE_PORTABILITY         : debugMessage.append("Portability")         ; break;
        case GL_DEBUG_TYPE_PERFORMANCE         : debugMessage.append("Performance")         ; break;
        case GL_DEBUG_TYPE_MARKER              : debugMessage.append("Marker")              ; break;
        case GL_DEBUG_TYPE_PUSH_GROUP          : debugMessage.append("Push Group")          ; break;
        case GL_DEBUG_TYPE_POP_GROUP           : debugMessage.append("Pop Group")           ; break;
        case GL_DEBUG_TYPE_OTHER               : debugMessage.append("Other")               ; break;
        default                                : debugMessage.append("Unknown")             ; break;
    }
    debugMessage.append(" | Severity: ");
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH         : debugMessage.append("High")        ; break;
        case GL_DEBUG_SEVERITY_MEDIUM       : debugMessage.append("Medium")      ; break;
        case GL_DEBUG_SEVERITY_LOW          : debugMessage.append("Low")         ; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION : debugMessage.append("Notification"); break;
        default                             : debugMessage.append("Unknown")     ; break;
    }
    debugMessage.append("\nMessage: ").append(message).append("\n");
    sDebugMessages.push_back(std::move(debugMessage));
    sErrorFlag = true;
}
