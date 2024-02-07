#pragma once

#include <brainfuck/backends/flat.hpp>

namespace brainfuck::flat::monolithic {
/// Runs a program contained in a fixed_flat_ast_t
/// container
template <auto const &Ast, size_t InstructionPos = 0>
void run(program_state_t &s) {
  constexpr flat_node_t const &Instr =
      Ast[InstructionPos];

  /// Single instruction
  if constexpr (std::holds_alternative<flat_token_t>(
                    Instr)) {
    constexpr flat_token_t const &Token =
        get<flat_token_t>(Instr);

    if constexpr (Token.token == pointer_increase_v) {
      ++s.i;
    } else if constexpr (Token.token ==
                         pointer_decrease_v) {
      --s.i;
    } else if constexpr (Token.token ==
                         pointee_increase_v) {
      s.data[s.i]++;
    } else if constexpr (Token.token ==
                         pointee_decrease_v) {
      s.data[s.i]--;
    } else if constexpr (Token.token == put_v) {
      std::putchar(s.data[s.i]);
    } else if constexpr (Token.token == get_v) {
      s.data[s.i] = std::getchar();
    }
  }

  /// Block of code (ie. whole program or while body)
  else if constexpr (std::holds_alternative<
                         flat_block_descriptor_t>(
                         Instr)) {
    constexpr flat_block_descriptor_t const
        &BlockDescriptor =
            get<flat_block_descriptor_t>(Instr);

    [&]<size_t... InstructionIDs>(
        std::index_sequence<InstructionIDs...>) {
      (run<Ast, 1 + InstructionPos + InstructionIDs>(
           s),
       ...);
    }(std::make_index_sequence<
        BlockDescriptor.size>{});
  }

  /// While loop
  else if constexpr (std::holds_alternative<
                         flat_while_t>(Instr)) {
    constexpr flat_while_t const &While =
        get<flat_while_t>(Instr);
    while (s.data[s.i]) {
      run<Ast, While.block_begin>(s);
    }
  }
}

} // namespace brainfuck::flat::monolithic
