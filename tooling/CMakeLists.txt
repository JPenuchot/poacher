add_executable(time-trace-wrapper time-trace-wrapper.cpp)

add_executable(benchmark-grapher benchmark-grapher.cpp ${IMPLOT_SOURCES})
conan_cmake_run(REQUIRES
  imgui/1.81
  nlohmann_json/3.9.0
  BASIC_SETUP
  BUILD all)

target_include_directories(benchmark-grapher PRIVATE
  ${IMPLOT_INCLUDE_DIR}
  ${CONAN_INCLUDE_DIRS_IMGUI}
  ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON})

target_link_directories(benchmark-grapher PRIVATE ${CONAN_LIB_DIRS_IMGUI})
target_link_libraries(benchmark-grapher ${CONAN_LIBS_IMGUI})
