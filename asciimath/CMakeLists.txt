cmake_minimum_required(VERSION 3.23)
project(asciimath)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

# Build
add_executable(asciimath ${SOURCES})
target_include_directories(asciimath PUBLIC include)
