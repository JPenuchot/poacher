find_package(ctbench REQUIRED)

# General benchmark parameters
set(SY_BENCH_BEGIN 0 CACHE STRING "Benchmark size begin")
set(SY_BENCH_END 20 CACHE STRING "Benchmark size end")
set(SY_BENCH_STEP 5 CACHE STRING "Benchmark size step")
set(SY_BENCH_SAMPLES 10 CACHE STRING "Benchmark iterations per size")

include_directories(../include)
include_directories(include)

# Clang with time-trace enabled
# add_compile_options(-ftime-trace -ftime-trace-granularity=1)

ctbench_add_benchmark(shunting-yard.addition-series.blaze-expression addition-series/blaze-expression.cpp ${SY_BENCH_BEGIN} ${SY_BENCH_END} ${SY_BENCH_STEP} ${SY_BENCH_SAMPLES})
ctbench_add_benchmark(shunting-yard.addition-series.text-formula addition-series/text-formula.cpp ${SY_BENCH_BEGIN} ${SY_BENCH_END} ${SY_BENCH_STEP} ${SY_BENCH_SAMPLES})
ctbench_add_benchmark(shunting-yard.addition-series.type-check addition-series/type-check.cpp ${SY_BENCH_BEGIN} ${SY_BENCH_END} ${SY_BENCH_STEP} 1)

# Graphs
ctbench_add_graph(shunting-yard.addition-series.graph compare_all.json
                  shunting-yard.addition-series.blaze-expression
                  shunting-yard.addition-series.text-formula)

ctbench_add_graph(shunting-yard.addition-series.check-graph compare_all.json
                  shunting-yard.addition-series.type-check
                  shunting-yard.addition-series.blaze-expression
                  shunting-yard.addition-series.text-formula)
