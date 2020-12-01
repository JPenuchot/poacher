#pragma once

#include <array>
#include <iostream>

#include <brainfuck/ast.hpp>
#include <brainfuck/ir.hpp>

namespace brainfuck {

struct program_state_t
{
  std::array<char, 30000> data;
  std::size_t i;
};

inline constexpr program_state_t initial_state = {{0}, 0};

template<typename... Ts> inline program_state_t & run(ir_block_t<Ts...>, program_state_t & s)
{
  (run(Ts{}, s), ...);
  return s;
}

template<typename... Ts> inline program_state_t & run(ir_while_t<Ts...>, program_state_t & s)
{
  while(s.data[s.i])
    (run(Ts{}, s), ...);
  return s;
}

inline program_state_t & run(ir_token_t<fwd_v>, program_state_t & s) {
  ++s.i;
  return s;
}

inline program_state_t & run(ir_token_t<bwd_v>, program_state_t & s) {
  --s.i;
  return s;
}

inline program_state_t & run(ir_token_t<inc_v>, program_state_t & s) {
  s.data[s.i]++;
  return s;
}

inline program_state_t & run(ir_token_t<dec_v>, program_state_t & s) {
  s.data[s.i]--;
  return s;
}

inline program_state_t & run(ir_token_t<put_v>, program_state_t & s) {
  std::putchar(s.data[s.i]);
  return s;
}

inline program_state_t & run(ir_token_t<get_v>, program_state_t & s) {
  s.data[s.i] = std::getchar();
  return s;
}

inline program_state_t & run(ir_token_t<nop_v>, program_state_t & s) {
  return s;
}

} // namespace brainfuck
