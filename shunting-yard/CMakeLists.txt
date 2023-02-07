cmake_minimum_required(VERSION 3.25)
project(shunting-yard-playground)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 20)

set(BUILD_TESTING ON CACHE BOOL "Enable or disable tests")

find_package(fmt REQUIRED)
find_package(kumi REQUIRED)
find_package(blaze REQUIRED)

link_libraries(fmt::fmt)
link_libraries(kumi::kumi)
link_libraries(blaze::blaze)

# Build
add_executable(shunting-yard ${SOURCES})
target_include_directories(shunting-yard PUBLIC include)

# if(BUILD_TESTING)
#   add_subdirectory(tests)
# endif(BUILD_TESTING)

include(cmake/docs.cmake)
