add_library(brainfuck_bench OBJECT main.cpp)
target_include_directories(brainfuck_bench PUBLIC "../include")

add_dependencies(brainfuck_bench time-trace-wrapper)
set_target_properties(brainfuck_bench
  PROPERTIES
    CXX_COMPILER_LAUNCHER
      "${CMAKE_BINARY_DIR}/tooling/time-trace-wrapper;brainfuck_bench.json"
  )
