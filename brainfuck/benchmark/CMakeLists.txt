find_package(ctbench REQUIRED)

set(BF_BENCH_BEGIN 1)
set(BF_BENCH_END 16)
set(BF_BENCH_STEP 1)
set(BF_BENCH_SAMPLES 1)

include_directories(../include)
include_directories(include)

# consecutive_loops
ctbench_add_benchmark(
  bfbench-consecutive_loops-et consecutive_loops/et.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(
  bfbench-consecutive_loops-flat consecutive_loops/flat.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_graph(bfbench-consecut3ive_loops compare.json
                  bfbench-consecutive_loops-et bfbench-consecutive_loops-flat)

# hello_world
# ctbench_add_benchmark(
#   bfbench-hello_world-et hello_world/et.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END}
#   ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
#
# ctbench_add_benchmark(
#   bfbench-hello_world-flat hello_world/flat.cpp ${BF_BENCH_BEGIN}
#   ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
#
# ctbench_add_graph(bfbench-hello_world compare.json bfbench-hello_world-et
#                   bfbench-hello_world-flat)

# imbricated_loops
ctbench_add_benchmark(
  bfbench-imbricated_loops-et imbricated_loops/et.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(
  bfbench-imbricated_loops-flat imbricated_loops/flat.cpp ${BF_BENCH_BEGIN}
  ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_graph(bfbench-imbricated_loops compare.json
                  bfbench-imbricated_loops-et bfbench-imbricated_loops-flat)
