#pragma once
#include "../Rendering/Renderer.h"
#include "../Rendering/IPipelineHandler.h"

#include "../Rendering/Mesh.h"
#include "../Rendering/Shader.h"

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

    static void APIENTRY debugOutput(unsigned int source, unsigned int type, unsigned int id, unsigned int severity,
                                     int length, const char* message, const void* userParam);
private:

    std::unique_ptr<Mesh> mDefaultMesh;
    std::unique_ptr<Shader> mDefaultShader;

    static std::vector<std::string> sDebugMessages;
    static bool sErrorFlag;
};

