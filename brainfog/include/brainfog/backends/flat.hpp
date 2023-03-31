#pragma once

// "flat" backend: flatten the AST into a vector,
// that can itself be turned into an array.
// The transformation is more of a proof of concept since
// this form is *very* close to its original one.

#include "brainfog/program.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <variant>
#include <vector>

#include <brainfog/ast.hpp>

namespace brainfog::flat {

using std::size_t;

// =============================================================================
// AST type definitions

/// Represents a single instruction token
struct flat_token_t {
  token_t token;
};

/// Block descriptor at the beginning of every block of adjacent instructions.
struct flat_block_descriptor_t {
  size_t size;
};

/// Represents a while instruction, pointing to another instruction block
struct flat_while_t {
  size_t block_begin;
};

using flat_node_t =
    std::variant<flat_token_t, flat_block_descriptor_t, flat_while_t>;

using flat_ast_t = std::vector<flat_node_t>;

template <size_t N> using fixed_flat_ast_t = std::array<flat_node_t, N>;

// =============================================================================
// generate_blocks overloads

/// Support structure for generate_blocks function
struct block_gen_status_t {
  /// Flat AST blocks, result of block_gen
  std::vector<flat_ast_t> blocks;

  /// Keeping track of which block is being generated
  size_t block_pos = 0;

  /// Keeping track of the size of the AST
  size_t total_size = 0;
};

/// Extracts an AST into a vector of blocks of contiguous operations
constexpr void block_gen(ast_node_ptr_t const &, block_gen_status_t &);

/// block_gen for a single AST token. Basically just a push_back.
constexpr void block_gen(ast_token_t const &tok, block_gen_status_t &s) {
  s.blocks[s.block_pos].push_back(flat_token_t{tok.get_token()});
}

/// block_gen for an AST block.
constexpr void block_gen(ast_block_t const &blo, block_gen_status_t &s) {
  // Save & update block pos before adding a block
  size_t const previous_pos = s.block_pos;
  s.block_pos = s.blocks.size();
  s.blocks.emplace_back();

  // Preallocating
  s.blocks[s.block_pos].reserve(blo.get_content().size() + 1);
  s.total_size += blo.get_content().size() + 1;

  // Adding block descriptor as a prefix
  s.blocks[s.block_pos].push_back(
      flat_block_descriptor_t{blo.get_content().size()});

  // Flattening instructions
  for (ast_node_ptr_t const &node : blo.get_content()) {
    block_gen(node, s);
  }

  // Restoring block pos after recursive block processing
  s.block_pos = previous_pos;
}

/// block_gen for a while instruction.
constexpr void block_gen(ast_while_t const &whi, block_gen_status_t &s) {
  s.blocks[s.block_pos].push_back(flat_while_t{s.blocks.size()});
  block_gen(whi.get_block(), s);
}

constexpr void block_gen(ast_node_ptr_t const &p, block_gen_status_t &s) {
  visit([&s](auto const &v) { block_gen(v, s); }, p);
}

// =============================================================================
// flatten implementation

constexpr flat_ast_t flatten(ast_node_ptr_t const &p) {
  flat_ast_t res;

  // Extracting as vector of blocks
  block_gen_status_t bgs;
  block_gen(p, bgs);

  std::vector<flat_ast_t> const &semi_res = bgs.blocks;
  res.reserve(bgs.total_size);

  std::vector<size_t> block_map;
  block_map.reserve(semi_res.size());

  // Step 1: flattening

  for (flat_ast_t const &block : semi_res) {
    // Updating block_map
    block_map.push_back(res.size());

    // Flattening instructions
    std::copy(block.begin(), block.end(), std::back_inserter(res));
  }

  // Step 2: linking

  for (flat_node_t &n : res) {
    if (std::holds_alternative<flat_while_t>(n)) {
      flat_while_t &w_ref = std::get<flat_while_t>(n);
      w_ref.block_begin = block_map[w_ref.block_begin];
    }
  }

  return res;
}

// =============================================================================
// Program execution

template <auto const &Ast, size_t InstructionPos = 0>
void run(program_state_t &s) {
  constexpr flat_node_t const &Instr = Ast[InstructionPos];

  if constexpr (std::holds_alternative<flat_token_t>(Instr)) {
    constexpr flat_token_t const &Token = std::get<flat_token_t>(Instr);

    if constexpr (Token.token == pointer_increase_v) {
      ++s.i;
    } else if constexpr (Token.token == pointer_decrease_v) {
      --s.i;
    } else if constexpr (Token.token == pointee_increase_v) {
      s.data[s.i]++;
    } else if constexpr (Token.token == pointee_decrease_v) {
      s.data[s.i]--;
    } else if constexpr (Token.token == put_v) {
      std::putchar(s.data[s.i]);
    } else if constexpr (Token.token == get_v) {
      s.data[s.i] = std::getchar();
    }
  }

  else if constexpr (std::holds_alternative<flat_block_descriptor_t>(Instr)) {
    constexpr flat_block_descriptor_t const &BlockDescriptor =
        std::get<flat_block_descriptor_t>(Instr);

    [&]<size_t... InstructionIDs>(std::index_sequence<InstructionIDs...>) {
      (run<Ast, 1 + InstructionPos + InstructionIDs>(s), ...);
    }
    (std::make_index_sequence<BlockDescriptor.size>{});
  }

  else if constexpr (std::holds_alternative<flat_while_t>(Instr)) {
    constexpr flat_while_t const &While = std::get<flat_while_t>(Instr);
    while (s.data[s.i]) {
      run<Ast, While.block_begin>(s);
    }
  }
}

} // namespace brainfog::flat
