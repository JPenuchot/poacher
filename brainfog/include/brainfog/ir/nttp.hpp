#pragma once

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

// NB: Does not work yet! Waiting for propconst: https://wg21.link/P1974

namespace brainfuck::backends::nttp {

template <ast_node_t const &n> void run_node(program_state_t &);

template <ast_token_t const &n> void run_token(program_state_t &s) {
  if constexpr (n.get_token() == fwd_v)
    s.i++;
  if constexpr (n.get_token() == bwd_v)
    s.i--;
  if constexpr (n.get_token() == inc_v)
    s.data[s.i]++;
  if constexpr (n.get_token() == dec_v)
    s.data[s.i]--;
  if constexpr (n.get_token() == put_v)
    std::putchar(s.data[s.i]);
  if constexpr (n.get_token() == get_v)
    s.data[s.i] = std::getchar();
}

template <ast_block_t const &n> void run_block(program_state_t &s) {
  constexpr auto N = n.get_content().size();
  [&s]<std::size_t... Is>(std::index_sequence<Is...>) {
    (run_node_ptr<n.get_content()[Is].get()>(s), ...);
  }
  (std::make_index_sequence<N>{});
}

template <ast_while_t const &n> void run_while(program_state_t &s) {
  while (s.data[s.i]) {
    run_block<&(n.get_block())>(s);
  }
}

template <ast_node_ptr_t const &p> void run_node_ptr(program_state_t &s) {
  if constexpr (p->get_kind() == ast_token_v)
    run_token<getas<ast_token_t>(p)>(s);
  if constexpr (p->get_kind() == ast_block_v)
    run_block<getas<ast_block_t>(p)>(s);
  if constexpr (p->get_kind() == ast_while_v)
    run_while<getas<ast_while_t>(p)>(s);
}

} // namespace brainfuck::backends::nttp
