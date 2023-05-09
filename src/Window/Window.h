#pragma once
#include <imgui.h>

#ifdef WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <filesystem>
#include <memory>
#include <string>

class Graph;
class Renderer;

class Window {
public:
    Window(std::string title, int width, int height);
    ~Window();

    void mainloop();

    [[nodiscard]] inline bool isInitSuccess() const {
        return mInitSuccess;
    }
private:
    void handleEvent(SDL_Event& e);
    void updateTitle();

    void drawMenu();
    void drawFileMenu();
    void drawEditMenu();

    void drawGraph();

    void loadConfig();
    void writeConfig();

    void loadGraph();
    void saveGraph() const;

    [[nodiscard]] static std::filesystem::path generateFilename();

    SDL_Window* mWindow = nullptr;
    SDL_GLContext mGLContext;

    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<Graph> mGraph;

    int mWidth, mHeight;

    std::string mTitle;

    bool mRunning = false;
    bool mInitSuccess = false;

    std::filesystem::path mGraphFilepath;
    bool mIsGraphFileWrittenTo = false;

    bool mCtrlHeld = false;
};
