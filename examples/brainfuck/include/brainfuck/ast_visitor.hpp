#pragma once

#include <tuple>

#include <brainfuck/ast.hpp>

namespace brainfuck {

// TODO: Intermediate representation ?
// - token_node_ir_t
// - block_node_ir_t
// - while_node_ir_t

template <typename... Ts> struct block_node_ir_t {
  constexpr block_node_ir_t(Ts const &...) {}
};
template <typename... Ts> struct while_node_ir_t {
  constexpr while_node_ir_t(Ts const &...) {}
};

template <ast_node_kind_t V>
using token_node_ir_t = std::integral_constant<ast_node_kind_t, V>;

template <ast_node_kind_t> struct kind_to_type {};
template <> struct kind_to_type<token_node_v> { using type = token_node_t; };
template <> struct kind_to_type<block_node_v> { using type = block_node_t; };
template <> struct kind_to_type<while_node_v> { using type = while_node_t; };

// f being a cosntexpr function with return type ast_node_t
template <typename f> constexpr auto to_ir(f) {
  constexpr ast_node_kind_t K = f{}()->get_kind();
  return to_ir(f{}, std::integral_constant<ast_node_kind_t, K>{});
}

// f being a cosntexpr function with return type token_node_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, token_node_v>) {
  constexpr token_t T = getas<token_node_t>(f{}())->get_token();
  return std::integral_constant<token_t, T>{};
}

// f being a cosntexpr function with return type block_node_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, block_node_v>) {
  constexpr std::size_t S = getas<block_node_t>(f{}())->get_content().size();
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return block_node_ir_t(to_ir([]() constexpr {
      return std::move(getas<block_node_t>(f{}())->get_content()[Is]);
    })...);
  }
  (std::make_index_sequence<S>{});
}

// f being a cosntexpr function with return type while_node_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, while_node_v>) {
  constexpr std::size_t S =
      getas<while_node_t>(f{}())->get_block().get_content().size();
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return while_node_ir_t(to_ir([]() constexpr {
      return std::move(
          getas<while_node_t>(f{}())->get_block().get_content()[Is]);
    })...);
  }
  (std::make_index_sequence<S>{});
}

} // namespace brainfuck
