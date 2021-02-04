file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "src/*.hpp")

# Build
add_executable(brainfuck ${SOURCES})
target_include_directories(brainfuck PUBLIC "include/")

# Format
add_custom_target(format-brainfuck COMMAND "clang-format" "-i"
  ${SOURCES} ${HEADERS})
poacher_format_register(format-brainfuck)
