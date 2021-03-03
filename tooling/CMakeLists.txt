add_executable(time-trace-wrapper time-trace-wrapper.cpp)

target_include_directories(time-trace-wrapper PRIVATE
  ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON})

add_subdirectory(benchmark-grapher)
