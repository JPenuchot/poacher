#pragma once

// "flat" backend: flatten the AST into a vector,
// that can itself be turned into an array.
// The transformation is more of a proof of concept
// since this form is *very* close to its original
// one.

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <variant>
#include <vector>

#include <brainfuck/ast.hpp>
#include <brainfuck/parser.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck::flat {

using std::size_t;

// ===============================================
// AST type definitions

/// Represents a single instruction token
struct flat_token_t {
  token_t token;
};

/// Block descriptor at the beginning of every block
/// of adjacent instructions.
struct flat_block_descriptor_t {
  size_t size;
};

/// Represents a while instruction, pointing to
/// another instruction block
struct flat_while_t {
  size_t block_begin;
};

/// Polymorphic representation of a node
using flat_node_t =
    std::variant<flat_token_t,
                 flat_block_descriptor_t,
                 flat_while_t>;

/// AST container type
using flat_ast_t = std::vector<flat_node_t>;

/// NTTP-compatible AST container type
template <size_t N>
using fixed_flat_ast_t = std::array<flat_node_t, N>;

// ===============================================
// generate_blocks overloads

/// Support structure for generate_blocks function
struct block_gen_state_t {
  /// Flat AST blocks, result of block_gen
  std::vector<flat_ast_t> blocks;

  /// Keeping track of which block is being generated
  size_t block_pos = 0;

  /// Keeping track of the size of the AST
  size_t total_size = 0;
};

/// Extracts an AST into a vector of blocks of
/// contiguous operations
constexpr void block_gen(ast_node_ptr_t const &,
                         block_gen_state_t &);

/// block_gen for a single AST token. Basically just a
/// push_back.
constexpr void block_gen(ast_token_t const &tok,
                         block_gen_state_t &s) {
  s.blocks[s.block_pos].push_back(
      flat_token_t{tok.token});
}

/// block_gen for an AST block.
constexpr void block_gen(ast_block_t const &blo,
                         block_gen_state_t &s) {
  // Save & update block pos before adding a block
  size_t const previous_pos = s.block_pos;
  s.block_pos = s.blocks.size();
  s.blocks.emplace_back();

  // Preallocating
  s.blocks[s.block_pos].reserve(blo.content.size() +
                                1);
  s.total_size += blo.content.size() + 1;

  // Adding block descriptor as a prefix
  s.blocks[s.block_pos].push_back(
      flat_block_descriptor_t{blo.content.size()});

  // Flattening instructions
  for (ast_node_ptr_t const &node : blo.content) {
    block_gen(node, s);
  }

  // Restoring block pos after recursive block
  // processing
  s.block_pos = previous_pos;
}

/// block_gen for a while instruction.
constexpr void block_gen(ast_while_t const &whi,
                         block_gen_state_t &s) {
  s.blocks[s.block_pos].push_back(
      flat_while_t{s.blocks.size()});
  block_gen(whi.block, s);
}

constexpr void block_gen(ast_node_ptr_t const &p,
                         block_gen_state_t &s) {
  visit([&s](auto const &v) { block_gen(v, s); }, p);
}

// ===============================================
// flatten implementation

constexpr flat_ast_t
flatten(ast_node_ptr_t const &parser_input) {
  flat_ast_t serialized_ast;

  // Extracting as vector of blocks
  block_gen_state_t bg_result;
  block_gen(parser_input, bg_result);

  // Small optimization to avoid reallcations
  serialized_ast.reserve(bg_result.total_size);

  // block_map[i] gives the index of
  // bg_result.blocks[i] in the serialized
  // representation
  std::vector<size_t> block_map;
  block_map.reserve(bg_result.blocks.size());

  // Step 1: flattening
  for (flat_ast_t const &block : bg_result.blocks) {
    // Updating block_map
    block_map.push_back(serialized_ast.size());

    // Appending instructions
    std::ranges::copy(block,
                      back_inserter(serialized_ast));
  }

  // Step 2: linking
  for (flat_node_t &node : serialized_ast) {
    if (holds_alternative<flat_while_t>(node)) {
      get<flat_while_t>(node).block_begin =
          block_map[get<flat_while_t>(node)
                        .block_begin];
    }
  }

  return serialized_ast;
}

/// Parses a BF program into a fixed_flat_ast_t value.
template <auto const &ProgramString>
constexpr auto parse_to_fixed_flat_ast() {
  // Getting AST vector size into a constexpr variable
  constexpr size_t AstArraySize =
      flatten(parser::parse_ast(ProgramString))
          .size();

  // Initializing static size array
  fixed_flat_ast_t<AstArraySize> arr;
  std::ranges::copy(
      flatten(parser::parse_ast(ProgramString)),
      arr.begin());

  return arr;
}

} // namespace brainfuck::flat
