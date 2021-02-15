#include <brainfuck/brainfuck.hpp>

constexpr auto const program_string =
    "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<"
    "-.<.+++.------.--------.>>+.>++.";

void foo() {
  brainfuck::program_state_t s{0, 0};
  run(brainfuck::to_ir(
          []() constexpr { return brainfuck::parse_ast(program_string); }),
      s);
}

int main() { foo(); }
