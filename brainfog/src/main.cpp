#include <brainfog/ast.hpp>
#include <brainfog/example_programs.hpp>
#include <brainfog/parsers/naive.hpp>
#include <brainfog/program.hpp>

// #define ET
// #define FLAT
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

static constexpr auto program_string = brainfog::example_programs::hello_world;

#ifdef FLAT
template <auto const &ProgramString> constexpr auto to_flat_ast() {
  namespace bf = brainfog;

  constexpr size_t AstArraySize =
      bf::flat::flatten(bf::naive_parser::parse_ast(ProgramString)).size();

  bf::flat::fixed_flat_ast_t<AstArraySize> arr;
  std::ranges::copy(
      bf::flat::flatten(bf::naive_parser::parse_ast(ProgramString)),
      arr.begin());

  return arr;
}
#endif

int main() {
  namespace bf = brainfog;

#ifdef ET
  { // Expression template backend
    using IR = decltype(bf::expression_template::to_et(
        []() { return bf::naive_parser::parse_ast(program_string); }));

    bf::program_state_t s;

    bf::expression_template::run(IR{}, s);
  }
#endif

#ifdef FLAT
  { // Flat backend
    static constexpr auto FlatAst = to_flat_ast<program_string>();
    bf::program_state_t s;
    bf::flat::run<FlatAst>(s);
  }
#endif

#ifdef PBG
  { // Pass by generator backend
    constexpr auto Generator = []() {
      return bf::naive_parser::parse_ast(program_string);
    };

    bf::program_state_t s;
    auto program = bf::pass_by_generator::codegen<Generator>();
    program(s);
  }
#endif
}
