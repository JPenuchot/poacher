#include <brainfog/ast.hpp>
#include <brainfog/example_programs.hpp>
#include <brainfog/parsers/naive.hpp>
#include <brainfog/program.hpp>

// #define FLAT
// #define ET
#define PBG

#ifdef ET
#include <brainfog/backends/expression_template.hpp>
#endif

#ifdef FLAT
#include <brainfog/backends/flat.hpp>
#endif

#ifdef PBG
#include <brainfog/backends/pass_by_generator.hpp>
#endif

static constexpr auto program_string = brainfog::example_programs::mandelbrot;

int main() {
  namespace bf = brainfog;

#ifdef ET
  { // Expression template backend
    bf::program_state_t s;

    bf::expression_template::codegen(bf::expression_template::to_et(
        []() { return bf::naive_parser::parse_ast(program_string); }))(s);
  }
#endif

#ifdef FLAT
  { // Flat backend
    static constexpr auto FlatAst =
        bf::flat::parse_to_fixed_flat_ast<program_string>();
    bf::program_state_t s;
    bf::flat::codegen<FlatAst>()(s);
  }
#endif

#ifdef PBG
  { // Pass by generator backend
    constexpr auto Generator = []() {
      return bf::naive_parser::parse_ast(program_string);
    };

    bf::program_state_t s;
    bf::pass_by_generator::codegen<Generator>()(s);
  }
#endif
}
