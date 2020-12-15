cmake_minimum_required(VERSION 3.10)
project(poacher)

# CMAKE OPTIONS

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("-fconstexpr-steps=2147483647" "-O3")

option(POACHER_METACLASS
  "Specifies whether metaclass code should be compiled."
  0)

if(POACHER_METACLASS)
  add_compile_options("-freflection")
endif()

include_directories("include")

# FORMATTING

file(GLOB_RECURSE HEADERS "include/*.hpp")
add_custom_target(format COMMAND "clang-format" "-i" "--style=Mozilla"
  ${HEADERS})
# EXAMPLES

add_subdirectory(examples)

# DOCUMENTATION

find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
set(DOXYGEN_EXTRACT_ALL "YES")
doxygen_add_docs(docs ALL)
