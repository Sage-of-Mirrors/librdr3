cmake_minimum_required(VERSION 3.8)
project(libdrawable)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LIBDRAWABLE_SRC
    "src/*.c"
    "include/*.h"
    "src/*.cpp"
    "src/ynv/*.cpp"
    "src/util/*.cpp"
    "include/*.hpp"
    "include/ynv/*.hpp"
    "include/util/*.hpp")

add_library(libdrawable ${LIBDRAWABLE_SRC})

target_include_directories(libdrawable PUBLIC include)

add_executable(tester test/main.cpp test/json.hpp)
target_link_libraries(tester libdrawable)