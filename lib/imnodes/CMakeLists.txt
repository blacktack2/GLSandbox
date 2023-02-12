cmake_minimum_required(VERSION 3.17)
project(ImNodes)

file(GLOB_RECURSE IMNODES_SRC *.cpp)
file(GLOB_RECURSE IMNODES_HEAD *.h)
add_library(imnodes ${IMNODES_SRC} ${IMNODES_HEAD})
