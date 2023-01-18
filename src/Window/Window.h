#pragma once
#include "imgui/imgui.h"

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

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

    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGLContext;
    ImGuiIO mIO;

    int mWidth, mHeight;

    bool mRunning = false;
    bool mInitSuccess = false;
};
