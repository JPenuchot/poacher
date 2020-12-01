#pragma once

#include <brainfuck/ast.hpp>
#include <brainfuck/ir.hpp>

namespace brainfuck {

// f being a cosntexpr function with return type ast_node_t
template <typename f> constexpr auto to_ir(f) {
  constexpr ast_node_kind_t K = f{}()->get_kind();
  return to_ir(f{}, std::integral_constant<ast_node_kind_t, K>{});
}

// f being a cosntexpr function with return type ast_token_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, token_node_v>) {
  constexpr token_t T = getas<ast_token_t>(f{}())->get_token();
  return ir_token_t<T>{};
}

// f being a cosntexpr function with return type ast_block_t
template <typename f>
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, block_node_v>) {
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
constexpr auto to_ir(f, std::integral_constant<ast_node_kind_t, while_node_v>) {
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

} // namespace brainfuck
