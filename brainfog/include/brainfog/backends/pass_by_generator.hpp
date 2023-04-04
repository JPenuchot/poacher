#pragma once

#include "brainfog/program.hpp"
#include <bits/utility.h>
#include <cstdio>
#include <type_traits>
#include <utility>

#include <brainfog/ast.hpp>

namespace brainfog::pass_by_generator {

template <auto Generator> constexpr auto codegen();

namespace detail {
/// Accepts a generator function that returns a vector of ast_node_ptr_t, and
/// generates its code.
template <auto Generator> constexpr auto vector_codegen() {
    // Generator should return an element of type ast_node_vec_t
  using GeneratorReturnType = decltype(Generator());
  static_assert(
      std::is_same_v<GeneratorReturnType, ast_node_vec_t>,
      "vector_codegen(): Invalid parameter Generator. Generator should be a "
      "function that returns a value of type ast_node_vec_t.");

  // Getting size as a constexpr value
  constexpr std::size_t Size = Generator().size();

  // Compile-time unrolling
  return [](program_state_t &state) {
    [&]<std::size_t... Indexes>(std::index_sequence<Indexes...>) {
      (codegen<[]() constexpr { return std::move(Generator()[Indexes]); }>()(
           state),
       ...);
    }
    (std::make_index_sequence<Size>{});
  };
}
} // namespace detail

template <auto Generator> constexpr auto codegen() {
  // Generator should return an element of type ast_node_ptr_t
  using GeneratorReturnType = decltype(Generator());
  static_assert(std::is_same_v<GeneratorReturnType, ast_node_ptr_t>,
                "codegen(): Invalid parameter Generator. Generator should be a "
                "function that returns a value of type ast_node_ptr_t.");

  // Getting kind as a constexpr variable
  constexpr ast_node_kind_t Kind = Generator()->get_kind();

  // Node is a single token
  if constexpr (Kind == ast_token_v) {
    // Getting token as a constexpr variable
    constexpr token_t Token =
        static_cast<ast_token_t const *>(Generator().get())->get_token();

    // Switching over the token's value
    if constexpr (Token == pointer_increase_v) {
      return [](program_state_t &state) { state.i++; };
    } else if constexpr (Token == pointer_decrease_v) {
      return [](program_state_t &state) { state.i--; };
    } else if constexpr (Token == pointee_increase_v) {
      return [](program_state_t &state) { state.data[state.i]++; };
    } else if constexpr (Token == pointee_decrease_v) {
      return [](program_state_t &state) { state.data[state.i]--; };
    } else if constexpr (Token == put_v) {
      return [](program_state_t &state) { std::putchar(state.data[state.i]); };
    } else if constexpr (Token == get_v) {
      return
          [](program_state_t &state) { state.data[state.i] = std::getchar(); };
    }
    // Other tokens should be unreachable.
    else {
      return [](program_state_t const &) {};
    }
  }

  // Node is a block
  else if constexpr (Kind == ast_block_v) {
    constexpr auto VectorGenerator = []() constexpr -> ast_node_vec_t {
      return std::move(
          static_cast<ast_block_t *>(Generator().get())->get_content());
    };
    return detail::vector_codegen<VectorGenerator>();
  }

  // Node is a while loop
  else if constexpr (Kind == ast_while_v) {
    constexpr auto VectorGenerator = []() constexpr -> ast_node_vec_t {
      return std::move(static_cast<ast_while_t *>(Generator().get())
                           ->get_block()
                           .get_content());
    };

    // Extracting the while body
    auto while_body = detail::vector_codegen<VectorGenerator>();

    // Encapsulating it inside a while loop
    return [while_body](program_state_t &state) {
      while (state.data[state.i]) {
        while_body(state);
      }
    };
  }
}

} // namespace brainfog::pass_by_generator
