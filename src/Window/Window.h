#pragma once
#include "imgui.h"

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <memory>

class Graph;
class Renderer;

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

    void drawGraph();

    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGLContext;
    ImGuiIO mIO;

    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<Graph> mGraph;

    int mWidth, mHeight;

    bool mRunning = false;
    bool mInitSuccess = false;
};
