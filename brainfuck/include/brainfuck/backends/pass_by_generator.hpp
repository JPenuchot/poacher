#pragma once

#include <cstdio>
#include <type_traits>
#include <utility>

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck::pass_by_generator {

/// Declaration of the `generates` concept which
/// is satisfied if Generator has an operator()
/// function that returns a value of type ReturnType.
template <typename Generator, typename ReturnType>
concept generates = requires(Generator gen) {
  { gen() } -> std::same_as<ReturnType>;
};

/// Code generation function.
/// It accepts a generator of ast_node_ptr_t
/// as a template parameter, and generates
/// the corresponding code for it.
template <generates<ast_node_ptr_t> auto>
constexpr auto codegen();

namespace detail {

/// Accepts a generator function that returns a vector
/// of ast_node_ptr_t, and generates its code.
template <generates<ast_node_vec_t> auto Generator>
constexpr auto vector_codegen() {
  // Getting size as a constexpr value
  constexpr std::size_t VectorSize =
      Generator().size();

  // Performing a static unrolling
  // on the vector's elements
  return [&](program_state_t &state) {
    [&]<std::size_t... IndexPack>(
        std::index_sequence<IndexPack...>) {
      (...,
       // A lambda has to be used here
       // for the program to compile
       [&]() {
         codegen<[]() -> ast_node_ptr_t {
           return std::move(Generator()[IndexPack]);
         }>()(state);
       }());
    }(std::make_index_sequence<VectorSize>{});
  };
}

/// This variable template is used
/// as a token to instruction map
template <token_t Token>
inline constexpr auto instruction_from_token =
    [](program_state_t &) {};

template <>
inline constexpr auto
    instruction_from_token<pointer_increase_v> =
        [](program_state_t &state) { state.i++; };
template <>
inline constexpr auto
    instruction_from_token<pointer_decrease_v> =
        [](program_state_t &state) { state.i--; };
template <>
inline constexpr auto
    instruction_from_token<pointee_increase_v> =
        [](program_state_t &state) {
          state.data[state.i]++;
        };
template <>
inline constexpr auto
    instruction_from_token<pointee_decrease_v> =
        [](program_state_t &state) {
          state.data[state.i]--;
        };
template <>
inline constexpr auto instruction_from_token<put_v> =
    [](program_state_t &state) {
      std::putchar(state.data[state.i]);
    };
template <>
inline constexpr auto instruction_from_token<get_v> =
    [](program_state_t &state) {
      state.data[state.i] = std::getchar();
    };
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
        static_cast<ast_token_t const &>(*Generator())
            .token;

    return detail::instruction_from_token<Token>;
  }

  // Node is a block
  else if constexpr (Kind == ast_block_v) {
    return detail::vector_codegen<
        []() -> ast_node_vec_t {
          return std::move(
              static_cast<ast_block_t &>(*Generator())
                  .content);
        }>();
  }

  // Node is a while loop
  else if constexpr (Kind == ast_while_v) {
    // Extracting the while body
    auto while_body = detail::vector_codegen<
        []() -> ast_node_vec_t {
          return std::move(
              static_cast<ast_while_t &>(*Generator())
                  .block.content);
        }>();

    // Encapsulating it inside a while loop
    return [while_body](program_state_t &state) {
      while (state.data[state.i]) {
        while_body(state);
      }
    };
  }
}

} // namespace brainfuck::pass_by_generator
