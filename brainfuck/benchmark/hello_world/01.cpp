#include <brainfuck/brainfuck.hpp>

constexpr auto const program_string =
    "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<"
    "-.<.+++.------.--------.>>+.>++.";

void bench_me() {
  brainfuck::program_state_t s{0, 0};

  constexpr auto ir = brainfuck::to_ir(
      []() constexpr { return brainfuck::parse_ast(program_string); });
  run(ir, s);
}
