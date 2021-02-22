# Add compile-time benchmark to compare compile-time data
add_custom_target(brainfuck_benchmark)

function(add_compile_benchmark target_name output file)
  add_library(${target_name} OBJECT ${file})
  target_include_directories(${target_name} PUBLIC "../include")
  add_dependencies(${target_name} time-trace-wrapper)

  set_target_properties(${target_name}
    PROPERTIES
      CXX_COMPILER_LAUNCHER
        "${CMAKE_BINARY_DIR}/tooling/time-trace-wrapper;${output}"
    )

  add_dependencies(brainfuck_benchmark ${target_name})
endfunction(add_compile_benchmark)

# Benchmark targets

add_compile_benchmark(__baseline baseline.json baseline.cpp)

add_compile_benchmark(__hello_world_1 hello_world/1.json hello_world/1.cpp)
add_compile_benchmark(__hello_world_2 hello_world/2.json hello_world/2.cpp)
add_compile_benchmark(__hello_world_3 hello_world/3.json hello_world/3.cpp)
add_compile_benchmark(__hello_world_4 hello_world/4.json hello_world/4.cpp)
add_compile_benchmark(__hello_world_5 hello_world/5.json hello_world/5.cpp)

add_compile_benchmark(__imbricated_loop_01 imbricated_loop/01.json imbricated_loop/01.cpp)
add_compile_benchmark(__imbricated_loop_02 imbricated_loop/02.json imbricated_loop/02.cpp)
add_compile_benchmark(__imbricated_loop_03 imbricated_loop/03.json imbricated_loop/03.cpp)
add_compile_benchmark(__imbricated_loop_04 imbricated_loop/04.json imbricated_loop/04.cpp)
add_compile_benchmark(__imbricated_loop_05 imbricated_loop/05.json imbricated_loop/05.cpp)
add_compile_benchmark(__imbricated_loop_10 imbricated_loop/10.json imbricated_loop/10.cpp)
add_compile_benchmark(__imbricated_loop_20 imbricated_loop/20.json imbricated_loop/20.cpp)
add_compile_benchmark(__imbricated_loop_30 imbricated_loop/30.json imbricated_loop/30.cpp)
add_compile_benchmark(__imbricated_loop_40 imbricated_loop/40.json imbricated_loop/40.cpp)

add_custom_target(benchmark_graph)
