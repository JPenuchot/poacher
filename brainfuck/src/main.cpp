#include <brainfuck/brainfuck.hpp>

constexpr auto const hello_world =
    "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<"
    "-.<.+++.------.--------.>>+.>++.";

void check(int) {}

int main() {
  namespace bf = brainfuck;

  bf::program_state_t state;
  state.i = 0;
  state.data = {0};

  run(bf::to_ir([]() constexpr { return bf::parse_ast(hello_world); }), state);

  // bf::backends::nttp::run_node_ptr<bf::parse_ast(hello_world)>(state);

  // auto ast = bf::parse_ast(hello_world);

  // consteval -> brainfuck::meta::gen_fragment<reflexpr(state)>(
  //    bf::parse_ast(hello_world));
}
