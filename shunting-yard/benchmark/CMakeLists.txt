find_package(ctbench REQUIRED)

# General benchmark parameters
set(SY_S_RANGE 1 3 1 CACHE STRING "Small range of benchmarks")
set(SY_M_RANGE 1 10 1 CACHE STRING "Medium range of benchmarks")
set(SY_L_RANGE 1 50 5 CACHE STRING "Large range of benchmarks")

set(SY_SAMPLES 10 CACHE STRING "Number of samples per benchmark")

include_directories(../include)
include_directories(include)

add_compile_options(-ftime-trace -ftime-trace-granularity=1)

# regular clang measurements
ctbench_add_benchmark_for_range(
  shunting-yard.addition-series.blaze-expression.clang
  addition-series/blaze-expression.cpp
  ${SY_M_RANGE} ${SY_SAMPLES})

ctbench_add_benchmark_for_range(
  shunting-yard.addition-series.text-formula.clang
  addition-series/text-formula.cpp
  ${SY_M_RANGE} ${SY_SAMPLES})

ctbench_add_benchmark_for_range(shunting-yard.addition-series.type-check.clang
  addition-series/type-check.cpp
  ${SY_M_RANGE} 1)

ctbench_add_graph(
  shunting-yard.addition-series.graph
  compare_ExecuteCompiler.json
  shunting-yard.addition-series.blaze-expression.clang
  shunting-yard.addition-series.text-formula.clang)

ctbench_add_graph(
  shunting-yard.addition-series.check
  debug.json
  shunting-yard.addition-series.type-check.clang
  shunting-yard.addition-series.blaze-expression.clang
  shunting-yard.addition-series.text-formula.clang)
