#pragma once
#include "../Rendering/Renderer.h"
#include "../Rendering/IPipelineHandler.h"

#include "../Rendering/Mesh.h"
#include "../Rendering/Shader.h"

#include "../Utils/TimerUtils.h"

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

#include <memory>

class GLSandboxRenderer final : public Renderer {
public:
    GLSandboxRenderer();
    ~GLSandboxRenderer() final = default;

    void update() final;
    void drawDebug() final;

    void resetPipeline() final;

    void loadAnalysisPipeline(unsigned int bulk, std::size_t iterations) final;

    static void APIENTRY debugOutput(unsigned int source, unsigned int type, unsigned int id, unsigned int severity,
                                     int length, const char* message, const void* userParam);
private:
    Timer mTimer{10};

    std::unique_ptr<Mesh> mQuad;
    std::unique_ptr<Shader> mDefaultShader, mProfileShader;

    static std::vector<std::string> sDebugMessages;
    static bool sErrorFlag;
};

