cmake_minimum_required(VERSION 3.10)
project(poacher)

include_directories("include")
set(CMAKE_CXX_STANDARD 20)

# Examples
add_subdirectory(examples)

# Formatting
file(GLOB_RECURSE HEADERS "include/*.hpp")
add_custom_target(format COMMAND "clang-format" "-i" "--style=Mozilla"
  ${HEADERS})
