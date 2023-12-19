#pragma once

#include <cerrno>
#include <cstdio>
#include <type_traits>
#include <utility>

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck::pass_by_generator {

template <typename Generator, typename ReturnType>
concept generates = requires(Generator gen) {
  { gen() } -> std::same_as<ReturnType>;
};

template <generates<ast_node_ptr_t> auto>
constexpr auto codegen();

namespace detail {
/// Accepts a generator function that returns a vector
/// of ast_node_ptr_t, and generates its code.
template <generates<ast_node_vec_t> auto VectorGenerator>
constexpr auto vector_codegen() {
  // Getting size as a constexpr value
  constexpr std::size_t Size =
      VectorGenerator().size();

  return [&](program_state_t &state) {
    [&]<std::size_t... Indexes>(
        std::index_sequence<Indexes...>) {
      (
          [&]() {
            codegen<[]() {
              return std::move(
                  VectorGenerator()[Indexes]);
            }>()(state);
          }(),
          ...);
    }(std::make_index_sequence<Size>{});
  };
}

template <token_t Token>
constexpr auto token_codegen() {
  if constexpr (Token == pointer_increase_v) {
    return [](program_state_t &state) { state.i++; };
  } else if constexpr (Token == pointer_decrease_v) {
    return [](program_state_t &state) { state.i--; };
  } else if constexpr (Token == pointee_increase_v) {
    return [](program_state_t &state) {
      state.data[state.i]++;
    };
  } else if constexpr (Token == pointee_decrease_v) {
    return [](program_state_t &state) {
      state.data[state.i]--;
    };
  } else if constexpr (Token == put_v) {
    return [](program_state_t &state) {
      std::putchar(state.data[state.i]);
    };
  } else if constexpr (Token == get_v) {
    return [](program_state_t &state) {
      state.data[state.i] = std::getchar();
    };
  } else {
    return [](program_state_t &) {};
  }
}

} // namespace detail

template <generates<ast_node_ptr_t> auto Generator>
constexpr auto codegen() {
  // Getting kind as a constexpr variable
  constexpr ast_node_kind_t Kind =
      Generator()->get_kind();

  // Node is a single token
  if constexpr (Kind == ast_token_v) {
    // Getting token as a constexpr variable
    constexpr token_t Token =
        static_cast<ast_token_t const *>(
            Generator().get())
            ->get_token();

    return detail::token_codegen<Token>();
  }

  // Node is a block
  else if constexpr (Kind == ast_block_v) {

    constexpr std::size_t Size =
        static_cast<ast_block_t &>(*Generator())
            .get_content()
            .size();

    return detail::vector_codegen<
        []() constexpr -> ast_node_vec_t {
          return std::move(static_cast<ast_block_t *>(
                               Generator().get())
                               ->get_content());
        }>();
  }

  // Node is a while loop
  else if constexpr (Kind == ast_while_v) {
    constexpr auto VectorGenerator =
        []() constexpr -> ast_node_vec_t {
      return std::move(static_cast<ast_while_t *>(
                           Generator().get())
                           ->get_block()
                           .get_content());
    };

    constexpr std::size_t Size =
        static_cast<ast_while_t *>(Generator().get())
            ->get_block()
            .get_content()
            .size();

    // Extracting the while body
    auto while_body =
        detail::vector_codegen<VectorGenerator>();

    // Encapsulating it inside a while loop
    return [while_body](program_state_t &state) {
      while (state.data[state.i]) {
        while_body(state);
      }
    };
  }
}

} // namespace brainfuck::pass_by_generator
