cmake_minimum_required(VERSION 3.25)
project(brainfuck)

set(COMPILE_BENCHMARKS
    OFF
    CACHE BOOL "Enable compile benchmarks")

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "src/*.hpp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -fconstexpr-steps=2147483647
  -fbracket-depth=2147483647
  -fconstexpr-depth=128)

# Build
add_executable(brainfuck ${SOURCES})

target_include_directories(brainfuck PUBLIC "include/")

# Format
add_custom_target(format-brainfuck COMMAND "clang-format" "-i" ${SOURCES}
                                          ${HEADERS})

# Benchmarks
if(COMPILE_BENCHMARKS)
  add_subdirectory(benchmark)
endif()
