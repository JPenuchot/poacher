#include <boost/process.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

const std::string_view CXX = "clang++";
const std::string_view CXX_FLAGS = "-O3";
std::string_view SOURCE_PATH = "main.cpp";

using benchmark_name = std::string_view;

struct benchmark_category {
  std::string_view name;
  std::vector<std::tuple<std::string_view, unsigned>> list;
};

benchmark_category hello_worlds{"hello_world",
                                {
                                    {"HELLO_WORLD_0", 0},
                                    {"HELLO_WORLD_1", 1},
                                    {"HELLO_WORLD_2", 2},
                                    {"HELLO_WORLD_3", 3},
                                }};

void run(benchmark_category const &c) {
  auto const &[name, benches] = c;
  for (auto const &b : benches) {
    auto const &[b_name, b_id] = b;
    std::ostringstream cmd;
    cmd << ' ' << CXX << ' ' << SOURCE_PATH << ' ' << CXX_FLAGS << ' '
        << "-ftime-trace" << ' ' << "-D" << b_name << ' ' << "-I../../include";
    std::cout << cmd.str() << '\n';
    boost::process::child c(cmd.str());
    c.wait();
  }
}

int main(int argc, char const *argv[]) {
  if (argc > 1)
    SOURCE_PATH = argv[1];
  run(hello_worlds);
  return 0;
}
