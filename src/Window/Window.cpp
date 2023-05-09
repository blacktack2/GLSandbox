#include "Window.h"

#include "../GLSandbox/Assets.h"
#include "../GLSandbox/GLSandboxRenderer.h"
#include "../GLSandbox/PipelineGraph.h"

#include "../Rendering/RenderConfig.h"

#include "../Utils/FileUtils.h"
#include "../Utils/SerializationUtils.h"

#include <glad/glad.h>

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <imgui_stdlib.h>

#include <utility>

static constexpr char gCONFIG_FILE[] = "GLSandbox.config";

static constexpr char gSERIAL_GRAPH_FILE[] = "File";

Window::Window(std::string title, int width, int height) :
mWidth(width), mHeight(height), mTitle(std::move(title)) {
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
    mWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, windowFlags);
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
    RenderConfig::setDefaultViewport(0, 0, mWidth, mHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(mWindow, mGLContext);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    std::filesystem::create_directory(getGraphAssetDirectory());
    std::filesystem::create_directory(getMeshAssetDirectory());
    std::filesystem::create_directory(getShaderAssetDirectory());
    std::filesystem::create_directory(getTextureAssetDirectory());

    mRenderer = std::make_unique<GLSandboxRenderer>();
    mGraph = std::make_unique<PipelineGraph>(*mRenderer);
    mGraph->addOnCleanEvent([this]() { updateTitle(); });
    mGraph->addOnDirtyEvent([this]() { updateTitle(); });

    mGraphFilepath = generateFilename();
    loadConfig();
    loadGraph();
    updateTitle();

    mInitSuccess = true;
}

Window::~Window() {
    writeConfig();

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

        ImUtils::begin();

        mRenderer->update();
        mRenderer->drawScene();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mWindow);
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        drawMenu();
        drawGraph();

        mRenderer->drawDebug();

        ImUtils::drawDataPanel();

        ImUtils::end();

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
                    RenderConfig::setDefaultViewport(0, 0, mWidth, mHeight);
                    break;
            }
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    mRunning = false;
                    break;
                case SDLK_LCTRL:
                    mCtrlHeld = true;
                    break;
                case SDLK_s:
                    if (mCtrlHeld)
                        saveGraph();
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (e.key.keysym.sym) {
                case SDLK_LCTRL:
                    mCtrlHeld = false;
                    break;
            }
            break;
    }
}

void Window::updateTitle() {
    std::string title = mTitle + " - " + mGraphFilepath.filename().generic_string() + (mGraph->isDirty() ? " *" : "");
    SDL_SetWindowTitle(mWindow, title.c_str());
}

void Window::drawMenu() {
    if (!ImGui::BeginMainMenuBar())
        return;

    drawFileMenu();
    drawEditMenu();

    ImGui::EndMainMenuBar();
}

void Window::drawFileMenu() {
    if (!ImGui::BeginMenu("File##MainMenu"))
        return;

    if (ImGui::MenuItem("New##MainMenu_File")) {
        mGraphFilepath = generateFilename();
        loadGraph();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Open##MainMenu_File")) {
        if (FileUtils::openFileDialog(mGraphFilepath, getGraphAssetDirectory(), getValidGraphFileExtensions())) {
            loadGraph();
        }
    }

    if (ImGui::MenuItem("Save##MainMenu_File"))
        saveGraph();
    if (ImGui::MenuItem("Save As##MainMenu_File")) {
        if (FileUtils::openSaveDialog(mGraphFilepath, getGraphAssetDirectory(), getValidGraphFileExtensions())) {
            if (mGraphFilepath.extension() != std::string(".").append(getGraphDefaultExtension())) {
                mGraphFilepath += ".";
                mGraphFilepath += getGraphDefaultExtension();
            }
            mGraph->regenerateID();
            saveGraph();
        }
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Exit##MainMenu_File"))
        mRunning = false;

    ImGui::EndMenu();
}

void Window::drawEditMenu() {
    if (!ImGui::BeginMenu("Edit##MainMenu"))
        return;

    if (ImGui::MenuItem("Clear##MainMenu_Edit"))
        mGraph->clearNodes();

    ImGui::EndMenu();
}

void Window::drawGraph() {
    mGraph->preDraw();
    mGraph->draw();
    mGraph->postDraw();
}

void Window::loadConfig() {
    std::ifstream stream(gCONFIG_FILE);
    if (!stream)
        return;

    while (stream.good()) {
        std::string dataID;
        stream >> dataID;
        if (dataID == gSERIAL_GRAPH_FILE)
            mGraphFilepath = SerializationUtils::readLine(stream);
        SerializationUtils::skipToNextLine(stream);
    }
}

void Window::writeConfig() {
    std::ofstream stream(gCONFIG_FILE);
    if (!stream)
        return;

    stream << gSERIAL_GRAPH_FILE << " " << mGraphFilepath.generic_string() << "\n";
}

void Window::loadGraph() {
    mRenderer->resetPipeline();
    mGraph->clearNodes();
    std::ifstream stream(mGraphFilepath, std::ios::in | std::ios::binary);
    if (stream)
        mGraph->deserialize(stream);
    else
        mGraph->initializeDefault();
    mGraph->markClean();
}

void Window::saveGraph() const {
    std::ofstream stream(mGraphFilepath);
    if (stream)
        mGraph->serialize(stream);
    mGraph->markClean();
}

std::filesystem::path Window::generateFilename() {
    return SerializationUtils::generateFilename(getGraphAssetDirectory(), "Graph", getGraphDefaultExtension());
}
