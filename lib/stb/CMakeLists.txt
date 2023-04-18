cmake_minimum_required(VERSION 3.17)
project(stb)

add_library(stb stb_image.h)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE C)
