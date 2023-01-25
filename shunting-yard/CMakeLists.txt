cmake_minimum_required(VERSION 3.23)
project(shunting-yard-playground)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

set(BUILD_TESTING ON CACHE BOOL "Enable or disable tests")

find_package(fmt REQUIRED)
link_libraries(fmt::fmt)

# Build
add_executable(shunting-yard ${SOURCES})
target_include_directories(shunting-yard PUBLIC include)

# if(BUILD_TESTING)
#   add_subdirectory(tests)
# endif(BUILD_TESTING)

include(cmake/docs.cmake)
