cmake_minimum_required(VERSION 3.8)
project(librdr3)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LIBRDR3_SRC
    "src/*.c"
    "src/*.cpp"
    "include/*.h"
    "include/*.hpp"

    "src/ynv/*.cpp"
    "include/ynv/*.hpp"

    "src/util/*.cpp"
    "include/util/*.hpp")

add_library(librdr3 ${LIBRDR3_SRC})

target_include_directories(librdr3 PUBLIC include)

add_executable(tester test/main.cpp test/json.hpp)
target_link_libraries(tester librdr3)