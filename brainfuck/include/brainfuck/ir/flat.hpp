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
#include <iterator>
#include <limits>
#include <optional>
#include <utility>
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

using flat_ast_t = cest::vector<flat_node_t>;

template <size_t N> using fixed_flat_ast_t = std::array<flat_node_t, N>;

// =============================================================================
// generate_blocks overloads

/// Support structure for generate_blocks function
struct block_gen_status_t {
  cest::vector<flat_ast_t> blocks;
  size_t block_pos = 0;
};

/// Extracts an AST into a vector of blocks of contiguous operations
constexpr void block_gen(ast_node_ptr_t const &p, block_gen_status_t &);

/// generate_blocksing for a single AST token. Basically just a push_back.
constexpr void block_gen(ast_token_t const &n, block_gen_status_t &status) {
  status.blocks[status.block_pos].push_back(flat_token_t{n.get_token()});
}

/// generate_blocksing for an AST block.
constexpr void block_gen(ast_block_t const &n, block_gen_status_t &status) {
  size_t const previous_pos = status.block_pos;

  status.block_pos = status.blocks.size();
  status.blocks.emplace_back();

  // Adding block descriptor as a prefix
  status.blocks[status.block_pos].push_back(
      flat_block_descriptor_t{n.get_content().size()});

  // Flattening instructions
  for (ast_node_ptr_t const &node : n.get_content()) {
    block_gen(node, status);
  }

  status.block_pos = previous_pos;
}

/// generate_blocksing for a while instruction. Create a new block, and flatten
/// it.
constexpr void block_gen(ast_while_t const &n, block_gen_status_t &status) {
  status.blocks[status.block_pos].push_back(flat_while_t{status.blocks.size()});
  block_gen(n.get_block(), status);
}

constexpr void block_gen(ast_node_ptr_t const &p, block_gen_status_t &status) {
  visit([&status](auto const &v) { block_gen(v, status); }, p);
}

// =============================================================================
// flatten implementation

constexpr flat_ast_t flatten(ast_node_ptr_t const &p) {
  flat_ast_t res;

  // Extracting as vector of blocks
  block_gen_status_t bgs;
  block_gen(p, bgs);

  cest::vector<flat_ast_t> semi_res = std::move(bgs.blocks);

  cest::vector<size_t> block_map;
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

  if constexpr (std::holds_alternative<flat_token_t>(Ast[InstructionPos])) {
    constexpr flat_token_t Token = std::get<flat_token_t>(Ast[InstructionPos]);

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
  }

  else if constexpr (std::holds_alternative<flat_block_descriptor_t>(
                         Ast[InstructionPos])) {
    constexpr flat_block_descriptor_t BlockDescriptor =
        std::get<flat_block_descriptor_t>(Ast[InstructionPos]);

    [&]<size_t... InstructionIDs>(std::index_sequence<InstructionIDs...>) {
      (run<Ast, 1 + InstructionPos + InstructionIDs>(s), ...);
    }
    (std::make_index_sequence<BlockDescriptor.size>{});
  }

  else if constexpr (std::holds_alternative<flat_while_t>(
                         Ast[InstructionPos])) {
    constexpr flat_while_t While = std::get<flat_while_t>(Ast[InstructionPos]);
    while (s.data[s.i]) {
      run<Ast, While.block_begin>(s);
    }
  }
}

} // namespace brainfuck::flat
