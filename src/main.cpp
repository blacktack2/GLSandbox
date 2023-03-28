#include "Window/Window.h"

int main(int argc, char* argv[]) {
    Window window("OpenGL Sandbox", 1080, 600);
    if (!window.isInitSuccess())
        return -1;
    window.mainloop();
    return 0;
}
