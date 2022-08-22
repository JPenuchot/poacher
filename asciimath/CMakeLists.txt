cmake_minimum_required(VERSION 3.23)
project(asciimath)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

set(BUILD_TESTING ON CACHE BOOL "Enable or disable tests")

find_package(fmt REQUIRED)

# Build
add_executable(asciimath ${SOURCES})
target_include_directories(asciimath PUBLIC include)
link_libraries(fmt::fmt)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

include(cmake/docs.cmake)
