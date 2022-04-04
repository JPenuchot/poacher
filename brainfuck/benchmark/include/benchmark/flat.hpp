#pragma once

#include <brainfuck/ir/flat.hpp>
#include <brainfuck/parsers/naive.hpp>

/// Parse program to flat AST
template <auto const &ProgramString> constexpr auto to_flat_ast() {
  namespace bf = brainfuck;
  namespace bflat = bf::flat;

  // Calculating size
  constexpr size_t AstArraySize =
      bflat::flatten(bf::naive_parser::parse_ast(ProgramString)).size();

  // Building array from vector
  bflat::flat_ast_t ast_vec =
      bflat::flatten(bf::naive_parser::parse_ast(ProgramString));

  bflat::fixed_flat_ast_t<AstArraySize> arr;
  std::ranges::copy(ast_vec, arr.begin());

  return arr;
}

/// Parse and run program using the flat AST backend
template <auto const &ProgramString> void run_program() {
  namespace bf = brainfuck;
  namespace bflat = bf::flat;

  static constexpr auto FlatAst = to_flat_ast<ProgramString>();

  bf::program_state_t s;

  bflat::run<FlatAst>(s);
}
