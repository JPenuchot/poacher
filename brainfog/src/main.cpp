#include <brainfog/ast.hpp>
#include <brainfog/example_programs.hpp>
#include <brainfog/parser.hpp>
#include <brainfog/program.hpp>

static constexpr auto program_string =
    brainfog::example_programs::hello_world;
namespace bf = brainfog;

// #define PBG
// #define ET
#define FLAT

#ifdef PBG
#include <brainfog/backends/pass_by_generator.hpp>

int main() {
  { // Pass by generator backend
    constexpr auto Generator = []() {
      return bf::parser::parse_ast(program_string);
    };

    bf::program_state_t s;
    bf::pass_by_generator::codegen<Generator>()(s);
  }
}
#endif

#ifdef ET
#include <brainfog/backends/expression_template.hpp>

int main() {
  bf::program_state_t s;

  auto expression_template =
      bf::expression_template::to_et([]() {
        return bf::parser::parse_ast(program_string);
      });

  bf::expression_template::codegen(
      expression_template)(s);
}
#endif

#ifdef FLAT
#include <brainfog/backends/flat.hpp>

int main() {
  static constexpr auto FlatAst =
      bf::flat::parse_to_fixed_flat_ast<
          program_string>();
  bf::program_state_t s;
  bf::flat::codegen<FlatAst>()(s);
}
#endif
