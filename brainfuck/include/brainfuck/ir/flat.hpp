#pragma once

// "flat" backend: flatten the AST into a vector,
// that can itself be turned into an array.
// The transformation is more of a proof of concept since
// this form is *very* close to its original one.

#include <algorithm>
#include <array>
#include <cstddef>
#include <variant>

#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck::flat {

using std::size_t;

// =============================================================================
// AST type definitions

struct flat_token_t {
  token_t token;
};

struct flat_block_descriptor_t {
  size_t begin;
  size_t end;
};

struct flat_while_t {
  flat_block_descriptor_t desc;
};

using flat_node_t = std::variant<flat_token_t, flat_while_t>;

using flat_ast_t = cest::vector<flat_node_t>;

template <size_t N> using fixed_flat_ast_t = std::array<flat_node_t, N>;

// =============================================================================
// add_offset overloads

// Default case: no offset is added
template <typename T> constexpr T add_offset(T v, size_t) { return v; }

// Block descriptor case: offset is added
constexpr flat_block_descriptor_t add_offset(flat_block_descriptor_t b,
                                             size_t offset) {
  return {.begin = b.begin + offset, .end = b.end + offset};
}

// While block case: offset is added to its block descriptor
constexpr flat_while_t add_offset(flat_while_t b, size_t offset) {
  return {.desc = add_offset(b.desc, offset)};
}

constexpr flat_node_t add_offset(flat_node_t n, size_t offset) {
  return std::visit(
      [offset](auto nn) -> flat_node_t { return add_offset(nn, offset); },
      std::move(n));
}

constexpr flat_ast_t add_offset(flat_ast_t ast, size_t offset) {
  for (flat_node_t &node : ast) {
    node = add_offset(std::move(node), offset);
  }
  return ast;
}

constexpr flat_ast_t concat(flat_ast_t a, flat_ast_t const &b) {
  size_t const offset = a.size();
  for (auto const &e : b) {
    a.push_back(add_offset(e, offset));
  }
  return a;
}

// =============================================================================
// flatten overloads

constexpr flat_ast_t flatten(ast_node_ptr_t const &p, flat_ast_t acc = {});

constexpr flat_ast_t flatten(ast_token_t const &n, flat_ast_t acc = {}) {
  acc.push_back(flat_token_t{n.get_token()});
  return acc;
}

constexpr flat_ast_t flatten(ast_block_t const &n, flat_ast_t acc = {}) {
  for (auto const &ast_ptr : n.get_content()) {
    acc = concat(std::move(acc), flatten(ast_ptr));
  }
  return acc;
}

constexpr flat_ast_t flatten(ast_while_t const &n, flat_ast_t acc = {}) {
  size_t while_pos = acc.size();

  // Pre-placing the while element
  acc.push_back(flat_while_t{{.begin = 0, .end = 0}});

  // Building block
  acc = flatten(n.get_block(), std::move(acc));

  // Updating while element
  acc[while_pos] =
      flat_node_t{flat_while_t{{.begin = while_pos + 1, .end = acc.size()}}};
  return acc;
}

constexpr flat_ast_t flatten(ast_node_ptr_t const &p, flat_ast_t acc) {
  return visit([&acc](auto const &v) { return flatten(v, std::move(acc)); }, p);
}

template <typename f> constexpr auto arrayfy(f) {
  constexpr size_t N = f{}().size();
  fixed_flat_ast_t<> res;

  flat_ast_t ast_vec = f{}();
  std::copy(ast_vec.begin(), ast_vec.end(), res.begin());

  return res;
}

} // namespace brainfuck::flat
