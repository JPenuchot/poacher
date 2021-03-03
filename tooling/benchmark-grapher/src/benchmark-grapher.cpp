#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>

#include <grapher/core.hpp>
#include <grapher/display.hpp>
#include <grapher/iostream.hpp>
#include <grapher/parsing.hpp>

int main(int argc, char const *argv[]) {
  constexpr int exec_id = 0;
  constexpr int out_id = 1;
  constexpr int res_begin_id = 2;
  const int res_end_id = argc;

  if (res_end_id <= res_begin_id) {
    std::cout << "Usage: " << argv[0] << " [OUTPUT] [INPUT FILES...]\n";
    return 1;
  }

  grapher::categories_t categories;
  for (auto i = res_begin_id; i < res_end_id; i++) {
    categories.push_back({argv[i], grapher::extract_category(argv[i])});
  }

  { // File output
    std::ofstream output_file(argv[out_id]);
    grapher::to_csv(output_file, categories);
  }

  grapher::graph(categories, "");

  return 0;
}
