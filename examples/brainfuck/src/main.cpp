#include <brainfuck/brainfuck.hpp>
#include <tuple>
#include <utility>

//constexpr auto const hello_world =
//    "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<"
//    "-.<.+++.------.--------.>>+.>++.";

constexpr auto const add_val = "[->+<]";

constexpr bool foo() {
  namespace bf = brainfuck;
  bf::ast_node_ptr_t ast(new bf::block_node_t(bf::parse_ast(add_val)));
  return true;
}

void czech(int) {}

int main() {
  namespace bf = brainfuck;
  constexpr auto ast_ir = bf::to_ir([]() constexpr {
    return bf::ast_node_ptr_t(new bf::block_node_t(bf::parse_ast(add_val)));
  });
  static_assert(foo());
  foo();

  czech(ast_ir);
}
