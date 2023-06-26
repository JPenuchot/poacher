#pragma once

// "expression_template" backend: C++20-compatible backend that turns the
// AST into an expression template using constexpr lambda construction to
// get around AST not being constexpr due to constexpr memory allocation.

// Issue: *terrible* performance as programs get a bit too big.

#include <brainfog/ast.hpp>
#include <brainfog/program.hpp>

#include <cstdio>

namespace brainfog::expression_template {

template <typename... Ts> struct et_block_t {
  constexpr et_block_t() {}
  constexpr et_block_t(Ts const &...) {}
};

template <typename... Ts> struct et_while_t {
  constexpr et_while_t() {}
  constexpr et_while_t(Ts const &...) {}
};

template <token_t V> struct et_token_t {};

// f being a cosntexpr function with return type ast_node_t
template <typename f> constexpr auto to_et(f);

// f being a cosntexpr function with return type ast_token_t
template <typename f>
constexpr auto to_et(f, std::integral_constant<ast_node_kind_t, ast_token_v>) {
  constexpr token_t T = getas<ast_token_t>(f{}())->get_token();
  return et_token_t<T>{};
}

// f being a cosntexpr function with return type ast_block_t
template <typename f>
constexpr auto to_et(f, std::integral_constant<ast_node_kind_t, ast_block_v>) {
  constexpr std::size_t S = getas<ast_block_t>(f{}())->get_content().size();
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return et_block_t(to_et([]() constexpr {
      return std::move(getas<ast_block_t>(f{}())->get_content()[Is]);
    })...);
  }
  (std::make_index_sequence<S>{});
}

// f being a cosntexpr function with return type ast_while_t
template <typename f>
constexpr auto to_et(f, std::integral_constant<ast_node_kind_t, ast_while_v>) {
  constexpr std::size_t S =
      getas<ast_while_t>(f{}())->get_block().get_content().size();
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return et_while_t(to_et([]() constexpr {
      return std::move(
          getas<ast_while_t>(f{}())->get_block().get_content()[Is]);
    })...);
  }
  (std::make_index_sequence<S>{});
}

template <typename f> constexpr auto to_et(f) {
  constexpr ast_node_kind_t K = f{}()->get_kind();
  return to_et(f{}, std::integral_constant<ast_node_kind_t, K>{});
}

// codegen meta-function overloads

template <typename... Ts> inline auto codegen(et_block_t<Ts...>) {
  return [](program_state_t &s) { (codegen(Ts{})(s), ...); };
}

template <typename... Ts> inline auto codegen(et_while_t<Ts...>) {
  return [](program_state_t &s) {
    while (s.data[s.i])
      (codegen(Ts{})(s), ...);
  };
}

inline auto codegen(et_token_t<pointer_increase_v>) {
  return [](program_state_t &s) { ++s.i; };
}

inline auto codegen(et_token_t<pointer_decrease_v>) {
  return [](program_state_t &s) { --s.i; };
}

inline auto codegen(et_token_t<pointee_increase_v>) {
  return [](program_state_t &s) { s.data[s.i]++; };
}

inline auto codegen(et_token_t<pointee_decrease_v>) {
  return [](program_state_t &s) { s.data[s.i]--; };
}

inline auto codegen(et_token_t<put_v>) {
  return [](program_state_t &s) { std::cout.put(s.data[s.i]); };
}

inline auto codegen(et_token_t<get_v>) {
  return [](program_state_t &s) { std::cin.get(s.data[s.i]); };
}

inline auto codegen(et_token_t<nop_v>) {
  return [](program_state_t &s) {};
}

} // namespace brainfog::expression_template
