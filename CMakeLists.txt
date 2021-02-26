cmake_minimum_required(VERSION 3.10)
project(poacher)

include(cmake/fetch_conan.cmake)
include(cmake/options.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("-fconstexpr-steps=2147483647" "-O3" "-fno-exceptions")

include_directories("include")

# FORMATTING

file(GLOB_RECURSE HEADERS "include/*.hpp")
add_custom_target(format)

function(poacher_format_register target_name)
  add_dependencies(format ${target_name})
endfunction()

# DOCUMENTATION

find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
set(DOXYGEN_EXTRACT_ALL "YES")
doxygen_add_docs(docs ALL)

add_subdirectory(tooling)
add_subdirectory(brainfuck)
add_subdirectory(asciimath)
