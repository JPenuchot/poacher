file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "src/*.hpp")

# Build
add_executable(asciimath ${SOURCES})
target_include_directories(asciimath PUBLIC "include/")

# Format
add_custom_target(format-asciimath COMMAND "clang-format" "-i"
  ${SOURCES} ${HEADERS})
poacher_format_register(format-asciimath)
