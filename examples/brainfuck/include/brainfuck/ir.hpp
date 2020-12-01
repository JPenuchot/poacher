#pragma once

namespace brainfuck {

template <typename... Ts> struct ir_block_t {
  constexpr ir_block_t() {}
  constexpr ir_block_t(Ts const &...) {}
};

template <typename... Ts> struct ir_while_t {
  constexpr ir_while_t() {}
  constexpr ir_while_t(Ts const &...) {}
};

template <token_t V> struct ir_token_t {};

} // namespace brainfuck
