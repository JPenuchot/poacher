cmake_minimum_required(VERSION 3.10)
project(poacher)

# CMAKE OPTIONS

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("include")

# FORMATTING

file(GLOB_RECURSE HEADERS "include/*.hpp")
add_custom_target(format COMMAND "clang-format" "-i" "--style=Mozilla"
  ${HEADERS})

# EXAMPLES

add_subdirectory(examples)

# DOCUMENTATION

find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
doxygen_add_docs(docs ALL)
