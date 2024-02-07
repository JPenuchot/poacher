#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace brainfuck {

//----------------------------------------------------
// TOKEN TYPE

/// Represents a Brainfuck token.
enum token_t : char {
  pointer_increase_v = '>', // ++ptr;
  pointer_decrease_v = '<', // --ptr;
  pointee_increase_v = '+', // ++*ptr;
  pointee_decrease_v = '-', // --*ptr;
  put_v = '.',              // putchar(*ptr);
  get_v = ',',              // *ptr=getchar();
  while_begin_v = '[',      // while (*ptr) {
  while_end_v = ']',        // }
  nop_v,                    // nop
};

/// Converts a char into its corresponding
/// Brainfuck token_t value.
constexpr enum token_t to_token(char c) {
  switch (c) {
  case pointer_increase_v:
    return pointer_increase_v;
  case pointer_decrease_v:
    return pointer_decrease_v;
  case pointee_increase_v:
    return pointee_increase_v;
  case pointee_decrease_v:
    return pointee_decrease_v;
  case put_v:
    return put_v;
  case get_v:
    return get_v;
  case while_begin_v:
    return while_begin_v;
  case while_end_v:
    return while_end_v;
  }
  return nop_v;
}

//----------------------------------------------------
// AST

/// Holds the underlaying node type
enum ast_node_kind_t : std::uint8_t {
  /// AST token node
  ast_token_v,
  /// AST block node
  ast_block_v,
  /// AST while node
  ast_while_v,
};

/// Parent class for any AST node type,
/// holds its type as an ast_node_kind_t
struct node_interface_t {
private:
  ast_node_kind_t kind_;

protected:
  constexpr node_interface_t(ast_node_kind_t kind)
      : kind_(kind){};

public:
  /// Returns the node kind tag.
  constexpr ast_node_kind_t get_kind() const {
    return kind_;
  }
  constexpr virtual ~node_interface_t() = default;
};
// Helpers

/// Token vector helper
using token_vec_t = std::vector<token_t>;

/// AST node pointer helper type
using ast_node_ptr_t =
    std::unique_ptr<node_interface_t>;

/// AST node vector helper type
using ast_node_vec_t = std::vector<ast_node_ptr_t>;

// !Helpers

/// AST node type for single Brainfuck tokens
struct ast_token_t : node_interface_t {
  token_t token;

  constexpr ast_token_t(token_t token_)
      : node_interface_t(ast_token_v), token(token_) {}
};

/// AST node type for Brainfuck code blocks
struct ast_block_t : node_interface_t {
  using node_ptr_t = ast_node_ptr_t;

  ast_node_vec_t content;

  constexpr ast_block_t(ast_node_vec_t &&content_)
      : node_interface_t(ast_block_v),
        content(std::move(content_)) {}

  constexpr ast_block_t(ast_block_t &&v) = default;
  constexpr ast_block_t &
  operator=(ast_block_t &&v) = default;

  constexpr ast_block_t(ast_block_t const &v) =
      delete;
  constexpr ast_block_t &
  operator=(ast_block_t const &v) = delete;
};

/// AST node type for Brainfuck while loop
struct ast_while_t : node_interface_t {
  ast_block_t block;

  constexpr ast_while_t(ast_block_t &&block_)
      : node_interface_t(ast_while_v),
        block(std::move(block_)) {}
};

template <typename F>
constexpr auto visit(F f, ast_node_ptr_t const &p) {
  switch (p->get_kind()) {
  case ast_token_v:
    return f(static_cast<ast_token_t const &>(*p));
  case ast_block_v:
    return f(static_cast<ast_block_t const &>(*p));
  case ast_while_v:
    return f(static_cast<ast_while_t const &>(*p));
  }
}

} // namespace brainfuck
