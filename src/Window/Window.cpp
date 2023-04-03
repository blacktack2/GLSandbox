#include "Window.h"

#include "../GLSandbox/Assets.h"
#include "../GLSandbox/GLSandboxRenderer.h"
#include "../GLSandbox/PipelineGraph.h"

#include <glad/glad.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_stdlib.h>

#include <filesystem>

Window::Window(const char *title, int width, int height) :
mWidth(width), mHeight(height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    auto windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, windowFlags);
    if (mWindow == nullptr)
        return;

    SDL_SetWindowMinimumSize(mWindow, 800, 600);

    mGLContext = SDL_GL_CreateContext(mWindow);

    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        return;

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLSandboxRenderer::debugOutput, nullptr);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, mWidth, mHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    mIO = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(mWindow, mGLContext);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    std::filesystem::create_directory(gMESH_ASSET_DIR);
    std::filesystem::create_directory(gSHADER_ASSET_DIR);

    mRenderer = std::make_unique<GLSandboxRenderer>();
    mGraph = std::make_unique<PipelineGraph>(*mRenderer);
    {
        std::ifstream stream("state.graph");
        if (stream)
            mGraph->deserialize(stream);
    }
    mInitSuccess = true;
}

Window::~Window() {
    {
        std::ofstream stream("state.graph");
        if (stream)
            mGraph->serialize(stream);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(mGLContext);
    if (mWindow) SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Window::mainloop() {
    SDL_Event e;

    mRunning = true;
    while (mRunning) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        while (SDL_PollEvent(&e))
            handleEvent(e);

        mRenderer->update();
        mRenderer->drawScene();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mWindow);
        ImGui::NewFrame();

        drawGraph();

        mRenderer->drawDebug();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(mWindow);
    }
}

void Window::handleEvent(SDL_Event &e) {
    ImGui_ImplSDL2_ProcessEvent(&e);

    switch (e.type) {
        case SDL_QUIT:
            mRunning = false;
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    mWidth  = e.window.data1;
                    mHeight = e.window.data2;
                    glViewport(0, 0, mWidth, mHeight);
                    break;
            }
            break;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        mRunning = false;
}

void Window::drawGraph() {
    mGraph->preDraw();
    mGraph->draw();
    mGraph->postDraw();
}
