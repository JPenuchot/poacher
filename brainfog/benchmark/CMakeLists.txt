find_package(ctbench REQUIRED)

# General benchmark parameters
set(BF_BENCH_BEGIN 1 CACHE STRING "Benchmark size begin")
set(BF_BENCH_END 100 CACHE STRING "Benchmark size end")
set(BF_BENCH_STEP 5 CACHE STRING "Benchmark size step")
set(BF_BENCH_SAMPLES 10 CACHE STRING "Benchmark iterations per size")

# Parameters specific to ET backend benchmarks
# These parameters were added to ensure benchmarks didn't take a decade to run.
set(BF_SLOW_BENCH_END 25 CACHE STRING "Benchmark size end for slow backends")
set(BF_SLOW_BENCH_STEP 10 CACHE STRING "Benchmark size step for slow backends")
set(BF_SLOW_BENCH_SAMPLES 5 CACHE STRING "Benchmark iterations per size for slow backends")
# set(BF_SLOW_HELLOWORLD_END 2 CACHE STRING "Hello World benchmark size end for slow backends")

include_directories(../include)
include_directories(include)

# Clang
# ctbench_set_compiler(clang++)
add_compile_options(-fconstexpr-steps=2147483647 -fbracket-depth=2147483647)

# ctbench_add_benchmark(bfbench-hello_world-flat-clang hello_world/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-hello_world-et-clang hello_world/et.cpp ${BF_BENCH_BEGIN} 2 ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-hello_world-pass_by_generator-clang hello_world/pass_by_generator.cpp ${BF_BENCH_BEGIN} 2 ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(bfbench-consecutive_loops-flat-clang consecutive_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-consecutive_loops-et-clang consecutive_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-consecutive_loops-pass_by_generator-clang consecutive_loops/pass_by_generator.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(bfbench-imbricated_loops-flat-clang imbricated_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-et-clang imbricated_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-pass_by_generator-clang imbricated_loops/pass_by_generator.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})

# Clang with time-trace enabled
ctbench_set_compiler(clang++)
add_compile_options(-ftime-trace -ftime-trace-granularity=1)

# ctbench_add_benchmark(bfbench-hello_world-flat-clang-tt hello_world/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-hello_world-et-clang-tt hello_world/et.cpp ${BF_BENCH_BEGIN} 3 ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})
# ctbench_add_benchmark(bfbench-hello_world-pass_by_generator-clang-tt hello_world/pass_by_generator.cpp ${BF_BENCH_BEGIN} 3 ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(bfbench-consecutive_loops-flat-clang-tt consecutive_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-consecutive_loops-et-clang-tt consecutive_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-consecutive_loops-pass_by_generator-clang-tt consecutive_loops/pass_by_generator.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})

ctbench_add_benchmark(bfbench-imbricated_loops-flat-clang-tt imbricated_loops/flat.cpp ${BF_BENCH_BEGIN} ${BF_BENCH_END} ${BF_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-et-clang-tt imbricated_loops/et.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})
ctbench_add_benchmark(bfbench-imbricated_loops-pass_by_generator-clang-tt imbricated_loops/pass_by_generator.cpp ${BF_BENCH_BEGIN} ${BF_SLOW_BENCH_END} ${BF_SLOW_BENCH_STEP} ${BF_BENCH_SAMPLES})

# Graphs

# Hello World

# ctbench_add_graph(bfbench-hello_world-clang-tt compare_all.json
#                   bfbench-hello_world-et-clang-tt
#                   bfbench-hello_world-flat-clang-tt
#                   bfbench-hello_world-pass_by_generator-clang-tt)

# ctbench_add_graph(bfbench-hello_world compare_all.json
#                   bfbench-hello_world-et-clang
#                   bfbench-hello_world-flat-clang
#                   bfbench-hello_world-pass_by_generator-clang)

# Consecutive

ctbench_add_graph(bfbench-consecutive_loops-clang-tt compare_all.json
                  bfbench-consecutive_loops-et-clang-tt
                  bfbench-consecutive_loops-flat-clang-tt
                  bfbench-consecutive_loops-pass_by_generator-clang-tt)

ctbench_add_graph(bfbench-consecutive_loops compare_all.json
                  bfbench-consecutive_loops-et-clang
                  bfbench-consecutive_loops-flat-clang
                  bfbench-consecutive_loops-pass_by_generator-clang)
# Imbricated

ctbench_add_graph(bfbench-imbricated_loops-clang-tt compare_all.json
                  bfbench-imbricated_loops-et-clang-tt
                  bfbench-imbricated_loops-flat-clang-tt
                  bfbench-imbricated_loops-pass_by_generator-clang-tt)

ctbench_add_graph(bfbench-imbricated_loops compare_all.json
                  bfbench-imbricated_loops-et-clang
                  bfbench-imbricated_loops-flat-clang
                  bfbench-imbricated_loops-pass_by_generator-clang)

# Imbricated vs consecutive

ctbench_add_graph(bfbench-imbricated_consecutive_loops-clang-tt compare_all.json
                  bfbench-consecutive_loops-et-clang-tt
                  bfbench-consecutive_loops-flat-clang-tt
                  bfbench-consecutive_loops-pass_by_generator-clang-tt
                  bfbench-imbricated_loops-et-clang-tt
                  bfbench-imbricated_loops-flat-clang-tt
                  bfbench-imbricated_loops-pass_by_generator-clang-tt)

ctbench_add_graph(bfbench-imbricated_consecutive_loops compare_all.json
                  bfbench-consecutive_loops-et-clang
                  bfbench-consecutive_loops-flat-clang
                  bfbench-consecutive_loops-pass_by_generator-clang
                  bfbench-imbricated_loops-et-clang
                  bfbench-imbricated_loops-flat-clang
                  bfbench-imbricated_loops-pass_by_generator-clang)
