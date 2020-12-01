#pragma once

namespace brainfuck {

template <typename... Ts> struct block_node_ir_t {
  constexpr block_node_ir_t(Ts const &...) {}
};
template <typename... Ts> struct while_node_ir_t {
  constexpr while_node_ir_t(Ts const &...) {}
};

template <ast_node_kind_t V>
using token_node_ir_t = std::integral_constant<ast_node_kind_t, V>;

} // namespace brainfuck
