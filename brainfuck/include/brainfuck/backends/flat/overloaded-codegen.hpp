#pragma once

#include <brainfuck/backends/flat.hpp>

namespace brainfuck::flat::overloaded {

// Necessary forward declaration
template <auto const &Ast,
          size_t InstructionPos = 0>
constexpr auto codegen();

/// Code generation implementation
/// for a single instruction
template <auto const &Ast,
          size_t InstructionPos = 0>
constexpr auto codegen(flat_token_t) {
  // Extracting token value
  constexpr flat_token_t Token =
      get<flat_token_t>(Ast[InstructionPos]);

  // Returning code for a single Brainfuck
  // instruction

  // >
  if constexpr (Token.token ==
                pointer_increase_v) {
    return [](program_state_t &s) { ++s.i; };
  }
  // <
  else if constexpr (Token.token ==
                     pointer_decrease_v) {
    return [](program_state_t &s) { --s.i; };
  }
  // +
  else if constexpr (Token.token ==
                     pointee_increase_v) {
    return
        [](program_state_t &s) { s.data[s.i]++; };

  }
  // -
  else if constexpr (Token.token ==
                     pointee_decrease_v) {
    return
        [](program_state_t &s) { s.data[s.i]--; };
  }
  // .
  else if constexpr (Token.token == put_v) {
    return [](program_state_t &s) {
      std::putchar(s.data[s.i]);
    };
  }
  // ,
  else if constexpr (Token.token == get_v) {
    return [](program_state_t &s) {
      s.data[s.i] = std::getchar();
    };
  }
}

/// Code generation implementation
/// for a code block
template <auto const &Ast,
          size_t InstructionPos = 0>
constexpr auto codegen(flat_block_descriptor_t) {
  return [](program_state_t &s) {
    // Generating an index sequence type
    // with a size equal to the code block size.
    // It will be passed to the template lambda
    // to expand its indexes.
    auto index_sequence =
        std::make_index_sequence<
            get<flat_block_descriptor_t>(
                Ast[InstructionPos])
                .size>{};

    // This lambda is used only to retrieve
    // the parameter pack generated above.
    [&]<size_t... Indexes>(
        std::index_sequence<Indexes...>) {
      // Expansion on the index to generate code
      // for each node and invoke it with the
      // program state
      (..., codegen<Ast, 1 + InstructionPos +
                             Indexes>()(s));
    }(index_sequence);
  };
}

/// Code generation implementation
/// for a while block
template <auto const &Ast,
          size_t InstructionPos = 0>
constexpr auto codegen(flat_while_t) {
  // Generating code for the while statement
  constexpr auto body =
      codegen<Ast, get<flat_while_t>(
                       Ast[InstructionPos])
                       .block_begin>();

  // Whole while expression
  return [body](program_state_t &s) {
    while (s.data[s.i]) {
      body(s);
    }
  };
}

/// Generic code generation entrypoint
template <auto const &Ast, size_t InstructionPos>
constexpr auto codegen() {
  constexpr flat_node_t Instr =
      Ast[InstructionPos];

  // Calling specialized codegen versions
  // using function overloading
  return codegen<Ast, InstructionPos>(
      decltype(get<Instr.index()>(Instr)){});
}
} // namespace brainfuck::flat::overloaded
