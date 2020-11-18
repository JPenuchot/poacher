#include <brainfuck/brainfuck.hpp>

constexpr bool foo() {
  auto ast = brainfuck::parse_ast(
      "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>"
      ".<-.<.+++.------.--------.>>+.>++.");
  return true;
}

int main() {
  static_assert(foo());
  foo();
}
