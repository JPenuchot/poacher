#pragma once

// "flat" backend: flatten the AST into a vector,
// that can itself be turned into an array.
// The transformation is more of a proof of concept since
// this form is *very* close to its original one.

#include "brainfuck/program.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <variant>

#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck::flat {

using std::size_t;

// =============================================================================
// AST type definitions

/// Represents a single instruction token
struct flat_token_t {
  token_t token;
};

struct flat_while_t {
  size_t begin;
  size_t end;
};

using flat_node_t = std::variant<flat_token_t, flat_while_t>;
using flat_ast_t = cest::vector<flat_node_t>;
template <size_t N> using fixed_flat_ast_t = std::array<flat_node_t, N>;

// =============================================================================
// flatten overloads

constexpr flat_ast_t flatten(ast_node_ptr_t const &p, flat_ast_t acc = {});

constexpr flat_ast_t flatten(ast_token_t const &n, flat_ast_t acc) {
  acc.push_back(flat_token_t{n.get_token()});
  return acc;
}

constexpr flat_ast_t flatten(ast_block_t const &n, flat_ast_t acc) {
  for (auto const &ast_ptr : n.get_content()) {
    acc = flatten(ast_ptr, std::move(acc));
  }
  return acc;
}

constexpr flat_ast_t flatten(ast_while_t const &n, flat_ast_t acc) {
  size_t while_pos = acc.size();

  // Pre-placing the while element
  acc.push_back(flat_while_t{.begin = 0, .end = 0});

  // Building block
  acc = flatten(n.get_block(), std::move(acc));

  // Updating while element
  acc[while_pos] =
      flat_node_t{flat_while_t{.begin = while_pos + 1, .end = acc.size()}};

  return acc;
}

constexpr flat_ast_t flatten(ast_node_ptr_t const &p, flat_ast_t acc) {
  return visit([&acc](auto const &v) { return flatten(v, std::move(acc)); }, p);
}

// =============================================================================
// Program execution

template <auto const &Ast, size_t InstructionId = 0, size_t End = Ast.size()>
inline void run(program_state_t &s) {
  if constexpr (InstructionId >= End) {
    // Stop condition
    return;
  } else {
    // Extracting current instruction
    constexpr flat_node_t Instruction = Ast[InstructionId];

    // Code execution
    if constexpr (std::holds_alternative<flat_while_t>(Instruction)) {
      constexpr flat_while_t While = std::get<flat_while_t>(Instruction);

      // Running while loop
      while (s.data[s.i]) {
        run<Ast, While.begin, While.end>(s);
      }

      // Run next instruction
      run<Ast, While.end, End>(s);

    } else if constexpr (std::holds_alternative<flat_token_t>(Instruction)) {
      constexpr flat_token_t Token = std::get<flat_token_t>(Instruction);

      // Instruction execution
      if constexpr (Token.token == fwd_v) {
        ++s.i;
      } else if constexpr (Token.token == bwd_v) {
        --s.i;
      } else if constexpr (Token.token == inc_v) {
        s.data[s.i]++;
      } else if constexpr (Token.token == dec_v) {
        s.data[s.i]--;
      } else if constexpr (Token.token == put_v) {
        std::putchar(s.data[s.i]);
      } else if constexpr (Token.token == get_v) {
        s.data[s.i] = std::getchar();
      }

      // Run next instruction
      run<Ast, InstructionId + 1, End>(s);
    }
  }
}

} // namespace brainfuck::flat
