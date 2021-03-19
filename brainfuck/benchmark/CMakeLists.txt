## Add compile-time benchmark to compare compile-time data
add_custom_target(brainfuck-benchmark)

function(add_compile_benchmark target_name output file)
  add_library(${target_name} OBJECT EXCLUDE_FROM_ALL ${file})
  target_include_directories(${target_name} PUBLIC "../include")
  add_dependencies(${target_name} time-trace-wrapper)

  set_target_properties(${target_name}
    PROPERTIES
      CXX_COMPILER_LAUNCHER
        "${CMAKE_BINARY_DIR}/bin/time-trace-wrapper;${output}"
    )

  add_dependencies(brainfuck-benchmark ${target_name})
endfunction(add_compile_benchmark)

## Add a whole folder to benchmark targets
function(add_benchmark_folder path)
  file(GLOB_RECURSE benchmark_sources "${path}/*")
  foreach(benchmark_source ${benchmark_sources})
    # Generating benchmark target name and output filename
    string(MD5 stem ${benchmark_source})
    string(REPLACE ".cpp" ".json" benchmark_output ${benchmark_source})

    # Adding benchmark target
    add_compile_benchmark("benchmark_${stem}"
      ${benchmark_output} ${benchmark_source})
  endforeach(benchmark_source)
endfunction(add_benchmark_folder)

# Benchmark targets
add_benchmark_folder(hello_world)
add_benchmark_folder(imbricated_loops)
add_benchmark_folder(consecutive_loops)

add_custom_target(benchmark-graph
  COMMAND
  "${CMAKE_BINARY_DIR}/bin/benchmark-grapher" "graphs"
    "hello_world"
    "imbricated_loops"
    "consecutive_loops"
  DEPENDS benchmark-grapher brainfuck-benchmark)
