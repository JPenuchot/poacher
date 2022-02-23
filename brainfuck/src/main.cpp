#include <brainfuck/ast.hpp>
#include <brainfuck/ir/expression_template.hpp>
#include <brainfuck/ir/flat.hpp>
#include <brainfuck/parsers/naive.hpp>
#include <brainfuck/program.hpp>
#include <cstddef>

constexpr const char *hello_world =
    "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<"
    "-.<.+++.------.--------.>>+.>++.";

template <auto const &Program> constexpr auto to_flat_ast() {
  namespace bf = brainfuck;
  namespace bflat = bf::flat;

  // Calculating size
  constexpr size_t N = bflat::flatten(bf::parse_ast(Program)).size();
  bflat::fixed_flat_ast_t<N> arr;

  // Building array from vector
  bflat::flat_ast_t ast_vec = bflat::flatten(bf::parse_ast(Program));
  std::copy(ast_vec.begin(), ast_vec.end(), arr.begin());

  return arr;
}

int main() { constexpr auto val = to_flat_ast<hello_world>(); }
