#include <brainfuck/ast.hpp>
#include <brainfuck/example_programs.hpp>
#include <brainfuck/ir/expression_template.hpp>
#include <brainfuck/ir/flat.hpp>
#include <brainfuck/parsers/naive.hpp>
#include <brainfuck/program.hpp>

template <auto const &ProgramString> constexpr auto to_flat_ast() {
  namespace bf = brainfuck;

  constexpr size_t AstArraySize =
      bf::flat::flatten(bf::naive_parser::parse_ast(ProgramString)).size();

  bf::flat::fixed_flat_ast_t<AstArraySize> arr;
  std::ranges::copy(
      bf::flat::flatten(bf::naive_parser::parse_ast(ProgramString)),
      arr.begin());

  return arr;
}

int main() {
  namespace bf = brainfuck;

  { // Expression template backend
    using IR = decltype(bf::expression_template::to_et([]() {
      return bf::naive_parser::parse_ast(
          brainfuck::example_programs::hello_world);
    }));

    bf::program_state_t s;

    bf::expression_template::run(IR{}, s);
  }

  { // Flat backend
    static constexpr auto FlatAst =
        to_flat_ast<brainfuck::example_programs::hello_world>();
    bf::program_state_t s;
    bf::flat::run<FlatAst>(s);
  }
}
