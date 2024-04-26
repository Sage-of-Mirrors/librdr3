cmake_minimum_required(VERSION 3.8)
project(librdr3)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE LIBRDR3_SRC
    "src/*.cpp"
    "include/*.h"
    "include/*.hpp"

    "src/navmesh/*.cpp"
    "include/navmesh/*.hpp"

    "src/util/*.cpp"
    "include/util/*.hpp"

    "src/drawable/*.cpp"
    "include/drawable/*.hpp"

    "src/collision/*.cpp"
    "include/collision/*.hpp"
	
	"src/audio/*.cpp"
	"include/audio/*.hpp"

    "src/crypto/*.cpp"
    "include/crypto/*.hpp"

    "src/fs/*.cpp"
    "include/fs/*.hpp"
)

add_library(librdr3 ${LIBRDR3_SRC})

target_include_directories(librdr3 PUBLIC include)
target_link_libraries(librdr3 PRIVATE OpenSSL::applink OpenSSL::Crypto)

add_executable(tester test/main.cpp test/json.hpp)
target_link_libraries(tester librdr3)