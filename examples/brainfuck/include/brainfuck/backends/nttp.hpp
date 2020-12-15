#pragma once

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck::backends::nttp {

template <const ast_node_t *n> void run_node(program_state_t &);

template <const ast_token_t *n> void run_token(program_state_t &s) {
  if constexpr (n->get_token() == fwd_v)
    s.i++;
  if constexpr (n->get_token() == bwd_v)
    s.i--;
  if constexpr (n->get_token() == inc_v)
    s.data[s.i]++;
  if constexpr (n->get_token() == dec_v)
    s.data[s.i]--;
  if constexpr (n->get_token() == put_v)
    std::putchar(s.data[s.i]);
  if constexpr (n->get_token() == get_v)
    s.data[s.i] = std::getchar();
}

template <const ast_block_t *n> void run_block(program_state_t &s) {
  constexpr auto N = n->get_content().size();
  [&s]<std::size_t... Is>(std::index_sequence<Is...>) {
    (run_node<n->get_content()[Is].get()>(s), ...);
  }
  (std::make_index_sequence<N>{});
}

template <const ast_while_t *n> void run_while(program_state_t &s) {
  while (s.data[s.i]) {
    run_block<&(n->get_block())>(s);
  }
}

template <const ast_node_t *n> void run_node(program_state_t &s) {
  if constexpr (n->get_kind() == ast_token_v)
    run_token<n>(s);
  if constexpr (n->get_kind() == ast_block_v)
    run_block<n>(s);
  if constexpr (n->get_kind() == ast_while_v)
    run_while<n>(s);
}

} // namespace brainfuck::backends::nttp
