project(brainfuck_benchmark LANGUAGES CXX)
add_executable(brainfuck_bench_launcher run.cpp)
target_include_directories(brainfuck_bench_launcher PUBLIC "../include/")

add_custom_target(brainfuck_bench
  COMMAND brainfuck_bench_launcher "${PROJECT_SOURCE_DIR}/main.cpp"
  WORKING_DIRECTORY "."
  USES_TERMINAL
  )
