find_package(ctbench REQUIRED)

set(BF_BENCH_BEGIN 1 CACHE STRING "Benchmark size begin")
set(BF_BENCH_END 8 CACHE STRING "Benchmark size end")
set(BF_BENCH_STEP 1 CACHE STRING "Benchmark size step")
set(BF_BENCH_SAMPLES 10 CACHE STRING "Benchmark iterations per size")

include_directories(../include)
include_directories(include)

# consecutive_loops
ctbench_add_benchmark(
  bfbench-consecutive_loops-et consecutive_loops/et.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(
  bfbench-consecutive_loops-flat consecutive_loops/flat.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_graph(bfbench-consecutive_loops compare.json
                  bfbench-consecutive_loops-et bfbench-consecutive_loops-flat)

# imbricated_loops
ctbench_add_benchmark(
  bfbench-imbricated_loops-et imbricated_loops/et.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(
  bfbench-imbricated_loops-flat imbricated_loops/flat.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_graph(bfbench-imbricated_loops compare.json
                  bfbench-imbricated_loops-et bfbench-imbricated_loops-flat)

# Imbricated vs consecutive
ctbench_add_graph(bfbench-imbricated_consecutive_loops compare.json
                  bfbench-consecutive_loops-et bfbench-consecutive_loops-flat
                  bfbench-imbricated_loops-et bfbench-imbricated_loops-flat)

ctbench_add_graph(bfbench-imbricated_consecutive_loops_all compare_all.json
                  bfbench-consecutive_loops-et bfbench-consecutive_loops-flat
                  bfbench-imbricated_loops-et bfbench-imbricated_loops-flat)

# hello_world

ctbench_add_benchmark(
  # Size capped at 2 for this one
  bfbench-hello_world-et hello_world/et.cpp ${BF_BENCH_BEGIN} 2
  ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(
  bfbench-hello_world-flat hello_world/flat.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_graph(bfbench-hello_world compare.json bfbench-hello_world-et
                  bfbench-hello_world-flat)
