#pragma once

#include <array>
#include <iostream>

#include <brainfuck/ast.hpp>
#include <brainfuck/ir.hpp>

namespace brainfuck {

struct program_state_t {
  std::array<char, 30000> data;
  std::size_t i;
};

template <typename... Ts>
[[gnu::always_inline]] void run(ir_block_t<Ts...>, program_state_t &s) {
  (run(Ts{}, s), ...);
}

template <typename... Ts>
[[gnu::always_inline]] void run(ir_while_t<Ts...>, program_state_t &s) {
  while (s.data[s.i])
    (run(Ts{}, s), ...);
}

[[gnu::always_inline]] void run(ir_token_t<fwd_v>, program_state_t &s) {
  ++s.i;
}

[[gnu::always_inline]] void run(ir_token_t<bwd_v>, program_state_t &s) {
  --s.i;
}

[[gnu::always_inline]] void run(ir_token_t<inc_v>, program_state_t &s) {
  s.data[s.i]++;
}

[[gnu::always_inline]] void run(ir_token_t<dec_v>, program_state_t &s) {
  s.data[s.i]--;
}

[[gnu::always_inline]] void run(ir_token_t<put_v>, program_state_t &s) {
  std::putchar(s.data[s.i]);
}

[[gnu::always_inline]] void run(ir_token_t<get_v>, program_state_t &s) {
  s.data[s.i] = std::getchar();
}

[[gnu::always_inline]] void run(ir_token_t<nop_v>, program_state_t &s) {}

} // namespace brainfuck
