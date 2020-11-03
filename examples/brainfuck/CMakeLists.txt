file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(brainfuck ${SOURCES})
target_include_directories(brainfuck PRIVATE "../../include")

add_custom_target(brainfuck-constexpr COMMAND "constexpr-everything"
  "-p" "${CMAKE_BINARY_DIR}" ${SOURCES})
