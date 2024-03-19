#define PBG 0
#define ET 1
#define FLAT_OVER 2
#define FLAT_MONO 3

#define BRAINFUCK_BACKEND ET

#if BRAINFUCK_BACKEND == PBG
#include <brainfuck/backends/pass_by_generator.hpp>
#endif
#if BRAINFUCK_BACKEND == ET
#include <brainfuck/backends/expression_template.hpp>
#endif
#if BRAINFUCK_BACKEND == FLAT_OVER
#include <brainfuck/backends/flat/overloaded-codegen.hpp>
#endif
#if BRAINFUCK_BACKEND == FLAT_MONO
#include <brainfuck/backends/flat/monolithic-codegen.hpp>
#endif

#include <brainfuck/example_programs.hpp>
#include <brainfuck/parser.hpp>

static constexpr auto program_string =
    brainfuck::example_programs::hello_world;
namespace bf = brainfuck;

#if BRAINFUCK_BACKEND == PBG
int main() {
  // Pass by generator backend
  bf::program_state_t s;
  auto code =
      bf::pass_by_generator::codegen<[]() constexpr {
        return std::move(
            bf::parser::parse_ast(program_string));
      }>();
  code(s);
}
#endif

#if BRAINFUCK_BACKEND == ET
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

#if BRAINFUCK_BACKEND == FLAT_OVER
int main() {
  static constexpr auto FlatAst =
      bf::flat::parse_to_fixed_flat_ast<
          program_string>(); // ok

  // Calling the monolithic implementation
  // {
  //   bf::program_state_t s;
  //   bf::flat::monolithic::codegen<FlatAst>()(s);
  // }

  // Calling the overloaded implementation
  {
    bf::program_state_t s;
    bf::flat::overloaded::codegen<FlatAst>()(s);
  }
}
#endif

#if BRAINFUCK_BACKEND == FLAT_MONO
int main() {
  static constexpr auto FlatAst =
      bf::flat::parse_to_fixed_flat_ast<
          program_string>(); // ok

  // Calling the monolithic implementation
  // {
  //   bf::program_state_t s;
  //   bf::flat::monolithic::codegen<FlatAst>()(s);
  // }

  // Calling the monolithic implementation
  {
    bf::program_state_t s;
    bf::flat::monolithic::codegen<FlatAst>()(s);
  }
}
#endif
