#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tuple>
#include <map>

#include <nlohmann/json.hpp>

using raw_entry_t = std::tuple<std::filesystem::path, nlohmann::json>;

struct entry_t {
  std::string name;

  int execute_compiler;
  int frontend;
  int source;
  int instantiate_function;
  int parse_class;
  int instantiate_class;
  int backend;
  int opt_module;
  int parse_template;
  int opt_function;
  int run_pass;
  int per_module_passes;
  int perform_pending_instantiations;
  int run_loop_pass;
  int code_gen_passes;
  int code_gen_function;
  int per_function_passes;
};

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

std::vector<entry_t> get_category_entries(std::filesystem::path const &cat) {
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
                   return extract_entry({p, js});
                 });
  return res;
}

using categories_t = std::map<std::string, std::vector<entry_t>>;

std::ostream &to_csv(std::ostream &o, entry_t const &e) {
  o << ',' << e.name << ',' << e.execute_compiler << ',' << e.frontend << ','
    << e.source << ',' << e.instantiate_function << ',' << e.parse_class << ','
    << e.instantiate_class << ',' << e.backend << ',' << e.opt_module << ','
    << e.parse_template << ',' << e.opt_function << ',' << e.run_pass << ','
    << e.per_module_passes << ',' << e.perform_pending_instantiations << ','
    << e.run_loop_pass << ',' << e.code_gen_passes << ',' << e.code_gen_function
    << ',' << e.per_function_passes;

  return o;
}

std::ostream &to_csv(std::ostream &o, categories_t const &cats) {
  o << "category,"
       "name,"
       "execute_compiler,"
       "frontend,"
       "source,"
       "instantiate_function,"
       "parse_class,"
       "instantiate_class,"
       "backend,"
       "opt_module,"
       "parse_template,"
       "opt_function,"
       "run_pass,"
       "per_module_passes,"
       "perform_pending_instantiations,"
       "run_loop_pass,"
       "code_gen_passes,"
       "code_gen_function,"
       "per_function_passes\n";
  for (auto const &[k_cat, v_cat] : cats) {
    for (auto const &e : v_cat) {
      to_csv((o << k_cat), e) << '\n';
    }
  }
  return o;
}

int main(int argc, char const *argv[]) {
  constexpr int exec_id = 0;
  constexpr int out_id = 1;
  constexpr int res_begin_id = 2;
  const int res_end_id = argc;

  if (res_end_id <= res_begin_id) {
    std::cout << "Usage: " << argv[0] << " [OUTPUT] [INPUT FILES...]\n";
    return 1;
  }

  categories_t entries_per_cat;
  for (auto i = res_begin_id; i < res_end_id; i++) {
    entries_per_cat[argv[i]] = get_category_entries(argv[i]);
  }

  { // File output
    std::ofstream output_file(argv[out_id]);
    to_csv(output_file, entries_per_cat);
  }
  return 0;
}
