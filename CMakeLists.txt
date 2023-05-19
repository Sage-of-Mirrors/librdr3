cmake_minimum_required(VERSION 3.8)
project(libdrawable)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LIBDRAWABLE_SRC
    "src/*.c"
    "include/*.h"
)

add_library(libdrawable ${LIBDRAWABLE_SRC})
set_target_properties(libdrawable PROPERTIES LINKER_LANGUAGE C)

target_include_directories(libdrawable PUBLIC include)

add_executable(tester test/main.c)
target_link_libraries(tester libdrawable)