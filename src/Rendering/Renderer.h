#pragma once
#include "Mesh.h"
#include "Shader.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

class Renderer {
public:
    typedef std::function<void()> pipeline_callback;

    Renderer();
    ~Renderer() = default;

    void update();
    void drawScene();
    void drawDebug();

    static void APIENTRY debugOutput(unsigned int source, unsigned int type, unsigned int id, unsigned int severity,
                                     int length, const char* message, const void* userParam);
private:
    std::vector<pipeline_callback> mRenderPipeline;

    std::unique_ptr<Mesh> mDefaultMesh;
    std::unique_ptr<Shader> mDefaultShader;

    static std::vector<std::string> sDebugMessages;
    static bool sErrorFlag;
};
