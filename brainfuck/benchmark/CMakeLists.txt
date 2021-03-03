# Add compile-time benchmark to compare compile-time data
add_custom_target(brainfuck_benchmark)

function(add_compile_benchmark target_name output file)
  add_library(${target_name} OBJECT EXCLUDE_FROM_ALL ${file})
  target_include_directories(${target_name} PUBLIC "../include")
  add_dependencies(${target_name} time-trace-wrapper)

  set_target_properties(${target_name}
    PROPERTIES
      CXX_COMPILER_LAUNCHER
        "${CMAKE_BINARY_DIR}/bin/time-trace-wrapper;${output}"
    )

  add_dependencies(brainfuck_benchmark ${target_name})
endfunction(add_compile_benchmark)

# Benchmark targets

add_compile_benchmark(__baseline baseline.json baseline.cpp)

add_compile_benchmark(__hello_world_01 hello_world/01.json hello_world/01.cpp)
add_compile_benchmark(__hello_world_02 hello_world/02.json hello_world/02.cpp)
add_compile_benchmark(__hello_world_03 hello_world/03.json hello_world/03.cpp)
add_compile_benchmark(__hello_world_04 hello_world/04.json hello_world/04.cpp)
add_compile_benchmark(__hello_world_05 hello_world/05.json hello_world/05.cpp)
add_compile_benchmark(__hello_world_06 hello_world/06.json hello_world/06.cpp)
add_compile_benchmark(__hello_world_07 hello_world/07.json hello_world/07.cpp)
add_compile_benchmark(__hello_world_08 hello_world/08.json hello_world/08.cpp)
add_compile_benchmark(__hello_world_09 hello_world/09.json hello_world/09.cpp)
add_compile_benchmark(__hello_world_10 hello_world/10.json hello_world/10.cpp)

add_compile_benchmark(__imbricated_loops_01 imbricated_loops/01.json imbricated_loops/01.cpp)
add_compile_benchmark(__imbricated_loops_02 imbricated_loops/02.json imbricated_loops/02.cpp)
add_compile_benchmark(__imbricated_loops_03 imbricated_loops/03.json imbricated_loops/03.cpp)
add_compile_benchmark(__imbricated_loops_04 imbricated_loops/04.json imbricated_loops/04.cpp)
add_compile_benchmark(__imbricated_loops_05 imbricated_loops/05.json imbricated_loops/05.cpp)
add_compile_benchmark(__imbricated_loops_10 imbricated_loops/10.json imbricated_loops/10.cpp)
add_compile_benchmark(__imbricated_loops_20 imbricated_loops/20.json imbricated_loops/20.cpp)
add_compile_benchmark(__imbricated_loops_30 imbricated_loops/30.json imbricated_loops/30.cpp)
add_compile_benchmark(__imbricated_loops_40 imbricated_loops/40.json imbricated_loops/40.cpp)

add_compile_benchmark(__consecutive_loops_01 consecutive_loops/01.json consecutive_loops/01.cpp)
add_compile_benchmark(__consecutive_loops_02 consecutive_loops/02.json consecutive_loops/02.cpp)
add_compile_benchmark(__consecutive_loops_03 consecutive_loops/03.json consecutive_loops/03.cpp)
add_compile_benchmark(__consecutive_loops_04 consecutive_loops/04.json consecutive_loops/04.cpp)
add_compile_benchmark(__consecutive_loops_05 consecutive_loops/05.json consecutive_loops/05.cpp)
add_compile_benchmark(__consecutive_loops_10 consecutive_loops/10.json consecutive_loops/10.cpp)
add_compile_benchmark(__consecutive_loops_20 consecutive_loops/20.json consecutive_loops/20.cpp)
add_compile_benchmark(__consecutive_loops_30 consecutive_loops/30.json consecutive_loops/30.cpp)
add_compile_benchmark(__consecutive_loops_40 consecutive_loops/40.json consecutive_loops/40.cpp)

add_custom_target(benchmark-graph
  COMMAND
  "${CMAKE_BINARY_DIR}/bin/benchmark-grapher" "benchmark-graph.svg"
    "baseline.json"
    "hello_world"
    "imbricated_loops"
    "consecutive_loops"
  DEPENDS benchmark-grapher brainfuck_benchmark)
