find_package(ctbench REQUIRED)

# General benchmark parameters
set(SY_RANGE 1 5 1 CACHE STRING "Small range of benchmarks")

set(SY_SAMPLES 10 CACHE STRING "Number of samples per benchmark")

include_directories(../include)
include_directories(include)

add_compile_options(-ftime-trace -ftime-trace-granularity=1)

ctbench_add_benchmark_for_range(
  shunting-yard.addition-series.blaze-expression
  addition-series/blaze-expression.cpp
  SY_RANGE ${SY_SAMPLES})

ctbench_add_benchmark_for_range(
  shunting-yard.addition-series.text-formula
  addition-series/text-formula.cpp
  SY_RANGE ${SY_SAMPLES})

ctbench_add_benchmark_for_range(
  shunting-yard.addition-series.formula-no-blaze
  addition-series/formula-no-blaze.cpp
  SY_RANGE ${SY_SAMPLES})

ctbench_add_benchmark_for_range(shunting-yard.addition-series.type-check
  addition-series/type-check.cpp
  SY_RANGE 1)

ctbench_add_graph(
  shunting-yard.addition-series.graph
  compare_ExecuteCompiler.json
  shunting-yard.addition-series.blaze-expression
  shunting-yard.addition-series.text-formula
  shunting-yard.addition-series.formula-no-blaze)
