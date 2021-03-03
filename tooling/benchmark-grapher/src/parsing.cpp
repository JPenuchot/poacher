#include <grapher/parsing.hpp>

#include <fstream>

namespace grapher {

entry_t extract_entry(raw_entry_t const &re) {
  entry_t res{
      .name{},
      .execute_compiler = -1,
      .frontend = -1,
      .source = -1,
      .instantiate_function = -1,
      .parse_class = -1,
      .instantiate_class = -1,
      .backend = -1,
      .opt_module = -1,
      .parse_template = -1,
      .opt_function = -1,
      .run_pass = -1,
      .per_module_passes = -1,
      .perform_pending_instantiations = -1,
      .run_loop_pass = -1,
      .code_gen_passes = -1,
      .code_gen_function = -1,
      .per_function_passes = -1,
  };

  auto const &[p, js] = re;
  res.name = p.filename().replace_extension("");

  for (auto e : js["traceEvents"]) {
    auto const &e_name = e["name"];

    if (e_name == "Total ExecuteCompiler") {
      res.execute_compiler = e["dur"];
    } else if (e_name == "Total Frontend") {
      res.frontend = e["dur"];
    } else if (e_name == "Total Source") {
      res.source = e["dur"];
    } else if (e_name == "Total InstantiateFunction") {
      res.instantiate_function = e["dur"];
    } else if (e_name == "Total ParseClass") {
      res.parse_class = e["dur"];
    } else if (e_name == "Total InstantiateClass") {
      res.instantiate_class = e["dur"];
    } else if (e_name == "Total Backend") {
      res.backend = e["dur"];
    } else if (e_name == "Total OptModule") {
      res.opt_module = e["dur"];
    } else if (e_name == "Total ParseTemplate") {
      res.parse_template = e["dur"];
    } else if (e_name == "Total OptFunction") {
      res.opt_function = e["dur"];
    } else if (e_name == "Total RunPass") {
      res.run_pass = e["dur"];
    } else if (e_name == "Total PerModulePasses") {
      res.per_module_passes = e["dur"];
    } else if (e_name == "Total PerformPendingInstantiations") {
      res.perform_pending_instantiations = e["dur"];
    } else if (e_name == "Total RunLoopPass") {
      res.run_loop_pass = e["dur"];
    } else if (e_name == "Total CodeGenPasses") {
      res.code_gen_passes = e["dur"];
    } else if (e_name == "Total CodeGen Function") {
      res.code_gen_function = e["dur"];
    } else if (e_name == "Total PerFunctionPasses") {
      res.per_function_passes = e["dur"];
    }
  }
  return res;
}

std::vector<entry_t> extract_category(std::filesystem::path const &cat) {
  namespace fs = std::filesystem;

  std::vector<fs::path> file_list =
      fs::is_directory(cat)
          ? std::vector<fs::path>{begin(fs::directory_iterator(cat)),
                                  end(fs::directory_iterator(cat))}
          : std::vector<fs::path>{cat};

  std::vector<entry_t> res;
  std::transform(file_list.begin(), file_list.end(), std::back_inserter(res),
                 [](fs::path const &p) -> entry_t {
                   std::ifstream f(p);
                   nlohmann::json js;
                   f >> js;
                   return extract_entry({p, std::move(js)});
                 });

  std::sort(res.begin(), res.end(),
            [](auto const &a, auto const &b) { return a.name < b.name; });
  return res;
}

} // namespace grapher
