find_package(ctbench REQUIRED)

# General benchmark parameters
set(BF_BENCH_BEGIN 1 CACHE STRING "Benchmark size begin")
set(BF_BENCH_END 10 CACHE STRING "Benchmark size end")
set(BF_BENCH_STEP 1 CACHE STRING "Benchmark size step")
set(BF_BENCH_SAMPLES 5 CACHE STRING "Benchmark iterations per size")

# Parameters specific to ET backend benchmarks
# These parameters were added to ensure benchmarks didn't take a decade to run.
set(BF_ET_BENCH_END 5 CACHE STRING "Expression template benchmark size end")
set(BF_ET_BENCH_STEP 1 CACHE STRING "Expression template benchmark size step")
set(BF_ET_BENCH_SAMPLES 2 CACHE STRING "Expression template benchmark iterations per size")

include_directories(../include)
include_directories(include)

# GCC
# ctbench_set_compiler(g++)

# ctbench_add_benchmark(bfbench-consecutive_loops-et-gcc consecutive_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_ET_BENCH_END} ${BF_ET_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-consecutive_loops-flat-gcc consecutive_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-imbricated_loops-et-gcc imbricated_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_ET_BENCH_END} ${BF_ET_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-imbricated_loops-flat-gcc imbricated_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

# Clang
# ctbench_set_compiler(clang++)
add_compile_options(-fconstexpr-steps=2147483647 -fbracket-depth=2147483647)

ctbench_add_benchmark(bfbench-consecutive_loops-et-clang consecutive_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_ET_BENCH_END} ${BF_ET_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-consecutive_loops-flat-clang consecutive_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-et-clang imbricated_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_ET_BENCH_END} ${BF_ET_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-flat-clang imbricated_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

# Clang with time-trace enabled
ctbench_set_compiler(clang++)
add_compile_options(-ftime-trace -ftime-trace-granularity=1)

ctbench_add_benchmark(bfbench-consecutive_loops-et-clang-tt consecutive_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_ET_BENCH_END} ${BF_ET_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-consecutive_loops-flat-clang-tt consecutive_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-et-clang-tt imbricated_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_ET_BENCH_END} ${BF_ET_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-flat-clang-tt imbricated_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})

# Graphs
ctbench_add_graph(bfbench-consecutive_loops-clang-tt compare_all.json
                  bfbench-consecutive_loops-et-clang-tt
                  bfbench-consecutive_loops-flat-clang-tt)

ctbench_add_graph(bfbench-consecutive_loops compare_all.json
                  # bfbench-consecutive_loops-et-gcc
                  # bfbench-consecutive_loops-flat-gcc
                  bfbench-consecutive_loops-et-clang
                  bfbench-consecutive_loops-flat-clang)

# Imbricated vs consecutive
ctbench_add_graph(bfbench-imbricated_consecutive_loops-clang-tt compare_all.json
                  bfbench-consecutive_loops-et-clang-tt
                  bfbench-consecutive_loops-flat-clang-tt
                  bfbench-imbricated_loops-et-clang-tt
                  bfbench-imbricated_loops-flat-clang-tt)

ctbench_add_graph(bfbench-imbricated_consecutive_loops compare_all.json
                  # bfbench-consecutive_loops-et-gcc
                  # bfbench-consecutive_loops-flat-gcc
                  # bfbench-imbricated_loops-et-gcc
                  # bfbench-imbricated_loops-flat-gcc
                  bfbench-consecutive_loops-et-clang
                  bfbench-consecutive_loops-flat-clang
                  bfbench-imbricated_loops-et-clang
                  bfbench-imbricated_loops-flat-clang)
