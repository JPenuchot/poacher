#pragma once

#include <cest/memory.hpp>
#include <vector>

namespace brainfog {

//------------------------------------------------------------------------------
// TOKEN TYPE

/// Represents a Brainfuck token.
enum token_t : char {
  /// ++ptr;
  fwd_v = '>',
  /// --ptr;
  bwd_v = '<',
  /// ++*ptr;
  inc_v = '+',
  /// --*ptr;
  dec_v = '-',
  /// putchar(*ptr);
  put_v = '.',
  /// *ptr=getchar();
  get_v = ',',
  /// while (*ptr) {
  whb_v = '[',
  /// }
  whe_v = ']',
  /// nop
  nop_v,
};

/// Converts a char into its corresponding Brainfuck token_t value.
constexpr enum token_t to_token(char c) {
  switch (c) {
  case fwd_v:
    return fwd_v;
  case bwd_v:
    return bwd_v;
  case inc_v:
    return inc_v;
  case dec_v:
    return dec_v;
  case put_v:
    return put_v;
  case get_v:
    return get_v;
  case whb_v:
    return whb_v;
  case whe_v:
    return whe_v;
  }
  return nop_v;
}

//------------------------------------------------------------------------------
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

/// Parent class for any AST node type, holds its type as an ast_node_kind_t
struct node_interface_t {
private:
  ast_node_kind_t kind_;

protected:
  constexpr node_interface_t(ast_node_kind_t kind) : kind_(kind){};

public:
  /// Returns the node kind tag.
  constexpr ast_node_kind_t get_kind() const { return kind_; }
  constexpr virtual ~node_interface_t() = default;
};

/// Helper class
template <typename Child> struct make_visitable_t {
  template <typename F> constexpr auto visit(F &&f) const {
    return f(*static_cast<Child const *>(this));
  }
};

// Helpers

/// Token vector helper
using token_vec_t = std::vector<token_t>;

/// AST node pointer helper type
using ast_node_ptr_t = cest::unique_ptr<node_interface_t>;

/// AST node vector helper type
using ast_node_vec_t = std::vector<ast_node_ptr_t>;

// !Helpers

/// AST node type for single Brainfuck tokens
struct ast_token_t : node_interface_t, make_visitable_t<ast_token_t> {
  token_t token_;

public:
  constexpr ast_token_t(token_t t) : node_interface_t(ast_token_v), token_(t) {}

  /// Returns the token's value.
  constexpr token_t get_token() const { return token_; }
};

/// AST node type for Brainfuck code blocks
struct ast_block_t : node_interface_t, make_visitable_t<ast_block_t> {
public:
  using node_ptr_t = ast_node_ptr_t;

private:
  ast_node_vec_t content_;

public:
  constexpr ast_block_t(ast_node_vec_t &&content)
      : node_interface_t(ast_block_v), content_(std::move(content)) {}

  constexpr ast_block_t(ast_block_t &&v) = default;
  constexpr ast_block_t &operator=(ast_block_t &&v) = default;

  constexpr ast_block_t(ast_block_t const &v) = delete;
  constexpr ast_block_t &operator=(ast_block_t const &v) = delete;

  /// Returns a const reference to its content.
  constexpr ast_node_vec_t const &get_content() const { return content_; }
  constexpr ast_node_vec_t &get_content() { return content_; }
};

/// AST node type for Brainfuck while loop
struct ast_while_t : node_interface_t, make_visitable_t<ast_while_t> {
  ast_block_t block_;

public:
  constexpr ast_while_t(ast_block_t &&block)
      : node_interface_t(ast_while_v), block_(std::move(block)) {}

  /// Returns a const reference to its block.
  constexpr ast_block_t const &get_block() const { return block_; }
  constexpr ast_block_t &get_block() { return block_; }
};

// isa implementation

/// Returns true if the AST node holds a node of specified type by looking up
/// its ast_node_kind_t element, otherwise false.
template <typename T> constexpr bool isa(node_interface_t const &n);

template <> constexpr bool isa<ast_token_t>(node_interface_t const &n) {
  return n.get_kind() == ast_token_v;
}

template <> constexpr bool isa<ast_block_t>(node_interface_t const &n) {
  return n.get_kind() == ast_block_v;
}

template <> constexpr bool isa<ast_while_t>(node_interface_t const &n) {
  return n.get_kind() == ast_while_v;
}

template <typename T, typename U> constexpr bool isa(U const *p) {
  return isa<U>(*p);
}

template <typename T, typename U>
constexpr bool isa(cest::unique_ptr<U> const &p) {
  return isa<T>(*p);
}

/// Casts a given pointer to the specified type if it matches() its kind tag,
/// otherwise returns nullptr.
template <typename T, typename U>
constexpr T *getas(cest::unique_ptr<U> const &p) {
  return isa<T>(p) ? static_cast<T *>(p.get()) : nullptr;
}

template <typename F> constexpr auto visit(F &&f, ast_node_ptr_t const &p) {
  switch (p->get_kind()) {
  case ast_token_v:
    return getas<ast_token_t>(p)->visit(f);
  case ast_block_v:
    return getas<ast_block_t>(p)->visit(f);
  case ast_while_v:
    return getas<ast_while_t>(p)->visit(f);
  }
}

} // namespace brainfog
