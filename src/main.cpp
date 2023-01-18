#include "Window/Window.h"

int main() {
    Window window("OpenGL Sandbox", 1200, 900);
    if (!window.isInitSuccess())
        return -1;
    window.mainloop();
    return 0;
}
