#pragma once

// "expression_template" backend: C++20-compatible
// backend that turns the AST into an expression
// template using constexpr lambda construction to get
// around AST not being constexpr due to constexpr
// memory allocation.

// Issue: *terrible* performance as programs get a bit
// too big.

#include <cstdio>

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck::expression_template {

template <typename... Nodes> struct et_block_t {
  constexpr et_block_t() {}
  constexpr et_block_t(Nodes const &...) {}
};

template <typename... Nodes> struct et_while_t {
  constexpr et_while_t() {}
  constexpr et_while_t(Nodes const &...) {}
};

template <token_t Token> struct et_token_t {};

// f being a constexpr function with return type
// ast_node_t
template <typename f> constexpr auto to_et(f);

// f being a constexpr function with return type
// ast_token_t
template <typename f>
constexpr auto
to_et(f, std::integral_constant<ast_node_kind_t,
                                ast_token_v>) {
  constexpr token_t T =
      static_cast<ast_token_t const &>(*f{}()).token;
  return et_token_t<T>{};
}

// f being a constexpr function with return type
// ast_block_t
template <typename f>
constexpr auto
to_et(f, std::integral_constant<ast_node_kind_t,
                                ast_block_v>) {
  constexpr std::size_t S =
      static_cast<ast_block_t const &>(*f{}())
          .content.size();
  return []<std::size_t... Is>(
             std::index_sequence<Is...>) {
    return et_block_t(to_et([]() constexpr {
      return std::move(
          static_cast<ast_block_t &>(*f{}())
              .content[Is]);
    })...);
  }(std::make_index_sequence<S>{});
}

// f being a constexpr function with return type
// ast_while_t
template <typename f>
constexpr auto
to_et(f, std::integral_constant<ast_node_kind_t,
                                ast_while_v>) {
  constexpr std::size_t S =
      static_cast<ast_while_t const &>(*f{}())
          .block.content.size();
  return []<std::size_t... Is>(
             std::index_sequence<Is...>) {
    return et_while_t(to_et([]() constexpr {
      return std::move(
          static_cast<ast_while_t &>(*f{}())
              .block.content[Is]);
    })...);
  }(std::make_index_sequence<S>{});
}

template <typename f> constexpr auto to_et(f) {
  constexpr ast_node_kind_t Kind = f{}()->get_kind();
  return to_et(f{},
               std::integral_constant<ast_node_kind_t,
                                      Kind>{});
}

// codegen meta-function overloads

template <typename... Ts>
inline auto codegen(et_block_t<Ts...>) {
  return [](program_state_t &state) {
    (codegen(Ts{})(state), ...);
  };
}

template <typename... Ts>
inline auto codegen(et_while_t<Ts...>) {
  return [](program_state_t &state) {
    while (state.data[state.i])
      (codegen(Ts{})(state), ...);
  };
}

inline auto codegen(et_token_t<pointer_increase_v>) {
  return [](program_state_t &state) { ++state.i; };
}

inline auto codegen(et_token_t<pointer_decrease_v>) {
  return [](program_state_t &state) { --state.i; };
}

inline auto codegen(et_token_t<pointee_increase_v>) {
  return [](program_state_t &state) {
    state.data[state.i]++;
  };
}

inline auto codegen(et_token_t<pointee_decrease_v>) {
  return [](program_state_t &state) {
    state.data[state.i]--;
  };
}

inline auto codegen(et_token_t<put_v>) {
  return [](program_state_t &state) {
    std::putchar(state.data[state.i]);
  };
}

inline auto codegen(et_token_t<get_v>) {
  return [](program_state_t &state) {
    state.data[state.i] = std::getchar();
  };
}

inline auto codegen(et_token_t<nop_v>) {
  return [](program_state_t &s) {};
}

} // namespace brainfuck::expression_template
