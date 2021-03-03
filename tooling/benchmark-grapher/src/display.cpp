#include <grapher/display.hpp>

#include <algorithm>
#include <iostream>
#include <string>

#include <sciplot/sciplot.hpp>

namespace grapher {

sciplot::Plot make_plot(category_t const &cat) {
  namespace sp = sciplot;

  sp::Plot plot;

  auto const &[name, entries] = cat;

  plot.xlabel("Benchmark Size Factor");
  plot.ylabel("CodeGen Function Time Per Size factor (ms)");

  std::vector<measure_t> x;
  std::vector<measure_t> y;

  // Feature list:
  // - execute_compiler
  // - frontend
  // - source
  // - instantiate_function
  // - parse_class
  // - instantiate_class
  // - backend
  // - opt_module
  // - parse_template
  // - opt_function
  // - run_pass
  // - per_module_passes
  // - perform_pending_instantiations
  // - run_loop_pass
  // - code_gen_passes
  // - code_gen_function
  // - per_function_passes

  for (auto const &e : entries) {
    auto const xx = std::atoi(e.name.c_str());
    if (xx == 0)
      continue;
    auto const yy = e.instantiate_function / xx;
    x.push_back(xx);
    y.push_back(yy);
  }

  plot.drawCurveWithPoints(x, y).label(name);

  return plot;
}

void graph(categories_t const &cats, std::filesystem::path const &p) {
  namespace sp = sciplot;
  std::vector<sp::Plot> plots;

  for (auto const &cat : cats)
    make_plot(cat).show();
}

} // namespace grapher
