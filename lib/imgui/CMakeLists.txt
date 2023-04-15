cmake_minimum_required(VERSION 3.17)
project(ImGui)

file(GLOB_RECURSE IMGUI_SRC imgui/*.cpp)
file(GLOB_RECURSE IMGUI_HEAD imgui/*.h)
file(GLOB_RECURSE NODES_SRC imgui-node-editor/*.cpp)
file(GLOB_RECURSE NODES_HEAD imgui-node-editor/*.h)

add_library(imgui ${IMGUI_SRC} ${IMGUI_HEAD} ${NODES_SRC} ${NODES_HEAD})
target_include_directories(imgui PUBLIC imgui)
target_include_directories(imgui PUBLIC imgui-node-editor)
