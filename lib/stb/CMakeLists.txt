cmake_minimum_required(VERSION 3.17)
project(STB)

file(GLOB_RECURSE STB_HEAD *.h)
add_library(stb INTERFACE ${STB_HEAD})
set_target_properties(stb PROPERTIES LINKER_LANGUAGE C)
