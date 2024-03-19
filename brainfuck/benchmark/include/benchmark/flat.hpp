#pragma once

#include <brainfuck/backends/flat/monolithic-codegen.hpp>
#include <brainfuck/parser.hpp>

/// Parse and run program using the flat AST backend
template <auto const &ProgramString>
inline void run_program() {
  static constexpr auto FlatAst =
      brainfuck::flat::parse_to_fixed_flat_ast<
          ProgramString>(); // ok

  brainfuck::program_state_t s;
  auto fun =
      brainfuck::flat::monolithic::codegen<FlatAst>();

  fun(s);
}
