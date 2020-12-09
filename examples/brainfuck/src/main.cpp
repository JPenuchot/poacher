#include <brainfuck/brainfuck.hpp>
#include <tuple>
#include <utility>

constexpr auto const hello_world =
    "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<"
    "-.<.+++.------.--------.>>+.>++.";

int main() {
  namespace bf = brainfuck;

  bf::program_state_t state;
  state.i = 0;
  state.data = {0};

  run(bf::to_ir([]() constexpr { return bf::parse_ast(hello_world); }), state);
}
