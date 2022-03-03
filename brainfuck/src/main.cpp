#include <brainfuck/ast.hpp>
#include <brainfuck/example_programs.hpp>
#include <brainfuck/ir/expression_template.hpp>
#include <brainfuck/ir/flat.hpp>
#include <brainfuck/parsers/naive.hpp>
#include <brainfuck/program.hpp>

template <auto const &ProgramString> constexpr auto to_flat_ast() {
  namespace bf = brainfuck;
  namespace bflat = bf::flat;

  // Calculating size
  constexpr size_t N = bflat::flatten(bf::parse_ast(ProgramString)).size();
  bflat::fixed_flat_ast_t<N> arr;

  // Building array from vector
  bflat::flat_ast_t ast_vec = bflat::flatten(bf::parse_ast(ProgramString));
  std::copy(ast_vec.begin(), ast_vec.end(), arr.begin());

  return arr;
}

constexpr auto FlatAst =
    to_flat_ast<brainfuck::example_programs::hello_world>();

int main() {
  namespace bf = brainfuck;
  namespace bflat = bf::flat;

  bf::program_state_t s;

  bflat::run<FlatAst>(s);
}
