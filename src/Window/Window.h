#pragma once
#include "../Rendering/Renderer.h"
#include "../GLSandbox/PipelineGraph.h"

#include "imgui.h"

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <memory>

class Window {
public:
    Window(const char* title, int width, int height);
    ~Window();

    void mainloop();

    [[nodiscard]] inline bool isInitSuccess() const {
        return mInitSuccess;
    }
private:
    void handleEvent(SDL_Event& e);

    void drawConfigPanel();
    void drawMeshConfig();
    void drawSkyboxConfig();
    void drawLightingConfig();

    void drawEditorPanel();

    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGLContext;
    ImGuiIO mIO;

    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<PipelineGraph> mGraph;

    int mWidth, mHeight;

    bool mRunning = false;
    bool mInitSuccess = false;
};
