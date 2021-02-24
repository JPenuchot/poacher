cmake_minimum_required(VERSION 3.10)
project(poacher)

# CMAKE OPTIONS DECLARATIONS

option(POACHER_TIDY
"Run clang-tidy"
OFF)

option(POACHER_METACLASS
"Specifies whether metaclass code should be compiled \
(requires a fragment-compatible compiler)"
OFF)

# COMPILE FLAGS

if(POACHER_TIDY)
  message("[WARNING] avoid running benchmarks with POACHER_TIDY turned on")
  set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
endif()

if(POACHER_METACLASS)
  message("[INFO] metaclass features enabled")
  add_compile_options("-freflection" "-DPOACHER_METACLASS=1")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("-fconstexpr-steps=2147483647" "-O3" "-fno-exceptions")

include_directories("include")

# FORMATTING

file(GLOB_RECURSE HEADERS "include/*.hpp")
add_custom_target(format)
#add_custom_target(format COMMAND "clang-format" "-i" "--style=Mozilla"
#  ${HEADERS})

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
