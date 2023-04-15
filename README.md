# OpenGL Sandbox

GLSandbox is an application designed to help play around with OpenGL rendering
pipelines using a graph-based editor interface.

## Installation

### Dependencies

This project makes use of the following dependencies

- [SDL2](https://www.libsdl.org/)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)
- [GLM](https://github.com/g-truc/glm)
- [OpenGL](https://www.khronos.org/opengl/)
- [nativefiledialog](https://github.com/mlabbe/nativefiledialog)

The SDL2 library must be built separately from this project as follows:

#### SDL2

##### Linux

Follow the instructions
[here](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php) to
download SDL2. If you download using apt-get SDL2 should be placed in the
expected directory by default. If you download manually you will have to ensure
SDL2 is located somewhere CMake can find it, otherwise you may want to edit the
CMakeLists.txt file to look in the correct directory.

##### Windows

Follow the instructions
[here](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php) to
download SDL2. CMake expects to find SDL2 in the c:/programs/sdl/ directory.

### Building

#### Linux

Change to the project root and type the following:

```bash
cmake --build .
```

This should create a new executable file called CellularAutomata. Running this
executable should start the application.

Note: You will need cmake version 3.17 or greater to build the application this
way.

#### Windows

This project can be built using cmake with VisualStudio (I have only tried using
version 2022 and do not know if other versions will work). It should be possible
to build using CLion, though as of yet I have not been able to get this working
due to issues with linking SDL2.

## Usage

This project is currently not yet at a minimum working example stage, and thus
is not developed enough to be used. This section will be updated once I have
at least implemented a basic example.
