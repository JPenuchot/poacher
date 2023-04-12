find_package(ctbench REQUIRED)

# General benchmark parameters
set(BF_S_RANGE 1 3 1 CACHE STRING "Small range of benchmarks")
set(BF_M_RANGE 1 10 1 CACHE STRING "Medium range of benchmarks")
set(BF_L_RANGE 1 50 5 CACHE STRING "Large range of benchmarks")

set(BF_SAMPLES 10 CACHE STRING "Number of samples per benchmark")

include_directories(../include)
include_directories(include)

# IMPORTANT NOTE: imbricated_loops benchmark must not be instantiated at sizes
# larger than 10 otherwise constexpr evaluation depth stops the compilation.

add_compile_options(-ftime-trace -ftime-trace-granularity=1)

ctbench_add_benchmark_for_range(
  bfbench-hello_world-flat
  hello_world/flat.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-hello_world-et
  hello_world/et.cpp
  BF_S_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-hello_world-pass_by_generator
  hello_world/pass_by_generator.cpp
  BF_S_RANGE ${BF_SAMPLES})

ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-flat
  consecutive_loops/flat.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-et
  consecutive_loops/et.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-pass_by_generator
  consecutive_loops/pass_by_generator.cpp
  BF_M_RANGE ${BF_SAMPLES})

ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-flat
  imbricated_loops/flat.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-et
  imbricated_loops/et.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-pass_by_generator
  imbricated_loops/pass_by_generator.cpp
  BF_M_RANGE ${BF_SAMPLES})

# Graphs

# Consecutive

ctbench_add_graph(
  bfbench-consecutive_loops
  compare_ExecuteCompiler.json
  bfbench-consecutive_loops-et
  bfbench-consecutive_loops-flat
  bfbench-consecutive_loops-pass_by_generator)

# Imbricated

ctbench_add_graph(
  bfbench-imbricated_loops
  compare_ExecuteCompiler.json
  bfbench-imbricated_loops-et
  bfbench-imbricated_loops-flat
  bfbench-imbricated_loops-pass_by_generator)

# Imbricated vs consecutive

ctbench_add_graph(
  bfbench-imbricated_consecutive_loops
  compare_ExecuteCompiler.json
  bfbench-consecutive_loops-et
  bfbench-consecutive_loops-flat
  bfbench-consecutive_loops-pass_by_generator
  bfbench-imbricated_loops-et
  bfbench-imbricated_loops-flat
  bfbench-imbricated_loops-pass_by_generator)
