#pragma once

// "expression_template" backend: C++20-compatible backend that turns the
// AST into an expression template using constexpr lambda construction to
// get around AST not being constexpr due to constexpr memory allocation.

// Issue: *terrible* performance as programs get a bit too big.

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

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

// f being a cosntexpr function with return type ast_node_t
template <typename f> constexpr auto to_ir(f) {
  constexpr ast_node_kind_t K = f{}()->get_kind();
  return to_ir(f{}, std::integral_constant<ast_node_kind_t, K>{});
}

// f being a cosntexpr function with return type ast_token_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, ast_token_v>) {
  constexpr token_t T = getas<ast_token_t>(f{}())->get_token();
  return ir_token_t<T>{};
}

// f being a cosntexpr function with return type ast_block_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, ast_block_v>) {
  constexpr std::size_t S = getas<ast_block_t>(f{}())->get_content().size();
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return ir_block_t(to_ir([]() constexpr {
      return std::move(getas<ast_block_t>(f{}())->get_content()[Is]);
    })...);
  }
  (std::make_index_sequence<S>{});
}

// f being a cosntexpr function with return type ast_while_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, ast_while_v>) {
  constexpr std::size_t S =
      getas<ast_while_t>(f{}())->get_block().get_content().size();
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return ir_while_t(to_ir([]() constexpr {
      return std::move(
          getas<ast_while_t>(f{}())->get_block().get_content()[Is]);
    })...);
  }
  (std::make_index_sequence<S>{});
}

// run meta-function overloads

template <typename... Ts>
inline void run(ir_block_t<Ts...>, program_state_t &s) {
  (run(Ts{}, s), ...);
}

template <typename... Ts>
inline void run(ir_while_t<Ts...>, program_state_t &s) {
  while (s.data[s.i])
    (run(Ts{}, s), ...);
}

inline void run(ir_token_t<fwd_v>, program_state_t &s) { ++s.i; }

inline void run(ir_token_t<bwd_v>, program_state_t &s) { --s.i; }

inline void run(ir_token_t<inc_v>, program_state_t &s) { s.data[s.i]++; }

inline void run(ir_token_t<dec_v>, program_state_t &s) { s.data[s.i]--; }

inline void run(ir_token_t<put_v>, program_state_t &s) {
  std::putchar(s.data[s.i]);
}

inline void run(ir_token_t<get_v>, program_state_t &s) {
  s.data[s.i] = std::getchar();
}

inline void run(ir_token_t<nop_v>, program_state_t &s) {}

} // namespace brainfuck
