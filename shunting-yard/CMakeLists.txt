cmake_minimum_required(VERSION 3.25)
project(shunting-yard-playground)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

set(BUILD_TESTING
    ON
    CACHE BOOL "Enable or disable tests")

find_package(fmt REQUIRED)
find_package(kumi REQUIRED)
find_package(blaze REQUIRED)

link_libraries(fmt::fmt)
link_libraries(kumi::kumi)
link_libraries(blaze::blaze)

add_compile_options(
  -Wall
  -Wextra
  -Werror
  -Wnull-dereference
  -Wold-style-cast
  -Wdouble-promotion
  -Wshadow
  -fno-rtti
  -march=native
  -Wfatal-errors)

# Build
add_executable(shunting-yard ${SOURCES})
target_include_directories(shunting-yard PUBLIC include)

include(cmake/docs.cmake)
