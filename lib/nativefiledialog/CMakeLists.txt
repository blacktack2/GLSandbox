cmake_minimum_required(VERSION 3.17)
project(nfd)

set(NFD_SRC src/include/nfd.h src/nfd_common.h src/nfd_common.c)

if (WIN32)
    list(APPEND NFD_SRC src/nfd_win.cpp)
elseif (APPLE)
    list(APPEND NFD_SRC src/nfd_cocoa.m)
elseif (UNIX)
    list(APPEND NFD_SRC src/nfd_gtk.c)
endif()

add_library(nfd STATIC ${NFD_SRC})

target_include_directories(nfd PUBLIC src/include)

if (UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    target_include_directories(nfd PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(nfd ${GTK3_LIBRARIES})
endif()