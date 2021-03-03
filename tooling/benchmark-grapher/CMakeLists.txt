file(GLOB_RECURSE BM_GRAPHER_SOURCES "src/*.cpp")
add_executable(benchmark-grapher ${BM_GRAPHER_SOURCES} ${IMPLOT_SOURCES})

target_include_directories(benchmark-grapher PRIVATE
  "include/"
  ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON})

find_package(sciplot REQUIRED)
target_link_libraries(benchmark-grapher sciplot::sciplot)
