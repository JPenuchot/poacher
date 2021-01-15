file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "src/*.hpp")

add_executable(brainfuck ${SOURCES})

target_include_directories(brainfuck PUBLIC "include/")

add_custom_target(constexpr-brainfuck COMMAND "constexpr-everything"
  "-p" "${CMAKE_BINARY_DIR}" ${SOURCES})

add_custom_target(format-brainfuck COMMAND "clang-format" "-i"
  ${SOURCES} ${HEADERS})

poacher_format_register(format-brainfuck)
