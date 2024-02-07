#pragma once

#include <brainfuck/backends/flat.hpp>

namespace brainfuck::flat::monolithic {
/// Generates a program from a fixed_flat_ast_t
template <auto const &Ast, size_t InstructionPos = 0>
constexpr auto codegen() {
  constexpr flat_node_t const &Instr =
      Ast[InstructionPos];

  /// Single instruction
  if constexpr (holds_alternative<flat_token_t>(
                    Instr)) {
    constexpr flat_token_t const &Token =
        get<flat_token_t>(Instr);

    if constexpr (Token.token == pointer_increase_v) {
      return [](program_state_t &s) { ++s.i; };
    } else if constexpr (Token.token ==
                         pointer_decrease_v) {
      return [](program_state_t &s) { --s.i; };
    } else if constexpr (Token.token ==
                         pointee_increase_v) {
      return
          [](program_state_t &s) { s.data[s.i]++; };

    } else if constexpr (Token.token ==
                         pointee_decrease_v) {
      return
          [](program_state_t &s) { s.data[s.i]--; };
    } else if constexpr (Token.token == put_v) {
      return [](program_state_t &s) {
        std::putchar(s.data[s.i]);
      };
    } else if constexpr (Token.token == get_v) {
      return [](program_state_t &s) {
        s.data[s.i] = std::getchar();
      };
    }
  }

  /// Block of code (ie. whole program or while body)
  else if constexpr (holds_alternative<
                         flat_block_descriptor_t>(
                         Instr)) {
    constexpr flat_block_descriptor_t const
        &BlockDescriptor =
            get<flat_block_descriptor_t>(Instr);
    return [](program_state_t &s) {
      [&]<size_t... InstructionIDs>(
          std::index_sequence<InstructionIDs...>) {
        (codegen<Ast, 1 + InstructionPos +
                          InstructionIDs>()(s),
         ...);
      }(std::make_index_sequence<
          BlockDescriptor.size>{});
    };

  }

  /// While loop
  else if constexpr (holds_alternative<flat_while_t>(
                         Instr)) {
    constexpr flat_while_t const &While =
        get<flat_while_t>(Instr);
    return [](program_state_t &s) {
      while (s.data[s.i]) {
        codegen<Ast, While.block_begin>()(s);
      }
    };
  }
}
} // namespace brainfuck::flat::monolithic
