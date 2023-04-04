find_package(ctbench REQUIRED)

# General benchmark parameters
set(BF_S_RANGE 1 3 1 CACHE STRING "Extra small range of benchmarks")
set(BF_M_RANGE 1 10 1 CACHE STRING "Small range of benchmarks")
set(BF_L_RANGE 1 50 5 CACHE STRING "Medium range of benchmarks")

set(BF_SAMPLES 10 CACHE STRING "Number of samples per benchmark")

include_directories(../include)
include_directories(include)

# Clang
# ctbench_set_compiler(clang++)
add_compile_options(-fconstexpr-steps=2147483647 -fbracket-depth=2147483647 -fconstexpr-depth=128)

ctbench_add_benchmark_for_range(
  bfbench-hello_world-flat-clang
  hello_world/flat.cpp
  BF_L_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-hello_world-et-clang
  hello_world/et.cpp
  BF_S_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-hello_world-pass_by_generator-clang
  hello_world/pass_by_generator.cpp
  BF_S_RANGE ${BF_SAMPLES})

ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-flat-clang
  consecutive_loops/flat.cpp
  BF_L_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-et-clang
  consecutive_loops/et.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-pass_by_generator-clang
  consecutive_loops/pass_by_generator.cpp
  BF_M_RANGE ${BF_SAMPLES})

ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-flat-clang
  imbricated_loops/flat.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-et-clang
  imbricated_loops/et.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-pass_by_generator-clang
  imbricated_loops/pass_by_generator.cpp
  BF_M_RANGE ${BF_SAMPLES})

# Clang with time-trace enabled
ctbench_set_compiler(clang++)
add_compile_options(-ftime-trace -ftime-trace-granularity=1)

ctbench_add_benchmark_for_range(
  bfbench-hello_world-flat-clang-tt
  hello_world/flat.cpp
  BF_L_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-hello_world-et-clang-tt
  hello_world/et.cpp
  BF_S_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-hello_world-pass_by_generator-clang-tt
  hello_world/pass_by_generator.cpp
  BF_S_RANGE ${BF_SAMPLES})

ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-flat-clang-tt
  consecutive_loops/flat.cpp
  BF_L_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-et-clang-tt
  consecutive_loops/et.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-consecutive_loops-pass_by_generator-clang-tt
  consecutive_loops/pass_by_generator.cpp
  BF_M_RANGE ${BF_SAMPLES})

ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-flat-clang-tt
  imbricated_loops/flat.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-et-clang-tt
  imbricated_loops/et.cpp
  BF_M_RANGE ${BF_SAMPLES})
ctbench_add_benchmark_for_range(
  bfbench-imbricated_loops-pass_by_generator-clang-tt
  imbricated_loops/pass_by_generator.cpp
  BF_M_RANGE ${BF_SAMPLES})

# Graphs

# Consecutive

ctbench_add_graph(
  bfbench-consecutive_loops-clang-tt
  compare_all.json
  bfbench-consecutive_loops-et-clang-tt
  bfbench-consecutive_loops-flat-clang-tt
  bfbench-consecutive_loops-pass_by_generator-clang-tt)

ctbench_add_graph(
  bfbench-consecutive_loops
  compare_all.json
  bfbench-consecutive_loops-et-clang
  bfbench-consecutive_loops-flat-clang
  bfbench-consecutive_loops-pass_by_generator-clang)
# Imbricated

ctbench_add_graph(
  bfbench-imbricated_loops-clang-tt
  compare_all.json
  bfbench-imbricated_loops-et-clang-tt
  bfbench-imbricated_loops-flat-clang-tt
  bfbench-imbricated_loops-pass_by_generator-clang-tt)

ctbench_add_graph(
  bfbench-imbricated_loops
  compare_all.json
  bfbench-imbricated_loops-et-clang
  bfbench-imbricated_loops-flat-clang
  bfbench-imbricated_loops-pass_by_generator-clang)

# Imbricated vs consecutive

ctbench_add_graph(
  bfbench-imbricated_consecutive_loops-clang-tt
  compare_all.json
  bfbench-consecutive_loops-et-clang-tt
  bfbench-consecutive_loops-flat-clang-tt
  bfbench-consecutive_loops-pass_by_generator-clang-tt
  bfbench-imbricated_loops-et-clang-tt
  bfbench-imbricated_loops-flat-clang-tt
  bfbench-imbricated_loops-pass_by_generator-clang-tt)

ctbench_add_graph(
  bfbench-imbricated_consecutive_loops
  compare_all.json
  bfbench-consecutive_loops-et-clang
  bfbench-consecutive_loops-flat-clang
  bfbench-consecutive_loops-pass_by_generator-clang
  bfbench-imbricated_loops-et-clang
  bfbench-imbricated_loops-flat-clang
  bfbench-imbricated_loops-pass_by_generator-clang)
