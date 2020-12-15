#pragma once

#include <cest/memory.hpp>
#include <cest/vector.hpp>

namespace brainfuck {

//------------------------------------------------------------------------------
// TOKEN TYPE

enum token_t : char {
  // (Program Start) char ptr[30000] = {0};
  fwd_v = '>', // ++ptr;
  bwd_v = '<', // --ptr;
  inc_v = '+', // ++*ptr;
  dec_v = '-', // --*ptr;
  put_v = '.', // putchar(*ptr);
  get_v = ',', // *ptr=getchar();
  whb_v = '[', // while (*ptr) {
  whe_v = ']', // }
  nop_v,       // nop
};

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

enum ast_node_kind_t : std::uint8_t {
  ast_token_v,
  ast_block_v,
  ast_while_v,
};

class ast_node_t {
private:
  ast_node_kind_t kind_;

protected:
  constexpr ast_node_t(ast_node_kind_t kind) : kind_(kind){};

public:
  constexpr ast_node_kind_t get_kind() const { return kind_; }
  constexpr virtual ~ast_node_t() = default;
};

// Helpers
using token_vec_t = cest::vector<token_t>;
using ast_node_ptr_t = cest::unique_ptr<ast_node_t>;
using ast_node_vec_t = cest::vector<ast_node_ptr_t>;
// !Helpers

class ast_token_t : public ast_node_t {
  token_t token_;

public:
  constexpr ast_token_t(token_t t) : ast_node_t(ast_token_v), token_(t) {}

  constexpr token_t get_token() const { return token_; }

  constexpr ~ast_token_t() = default;
};

class ast_block_t : public ast_node_t {
public:
  using node_ptr_t = ast_node_ptr_t;

private:
  ast_node_vec_t content_;

public:
  constexpr ast_block_t(ast_node_vec_t &&content)
      : ast_node_t(ast_block_v), content_(std::move(content)) {}

  constexpr ast_block_t(ast_block_t &&v) = default;
  constexpr ast_block_t(ast_block_t const &v) = delete;

  constexpr ast_block_t &operator=(ast_block_t &&v) = default;
  constexpr ast_block_t &operator=(ast_block_t const &v) = delete;

  constexpr ast_node_vec_t &get_content() { return content_; }
  constexpr ast_node_vec_t const &get_content() const { return content_; }

  constexpr ~ast_block_t() = default;
};

class ast_while_t : public ast_node_t {
  ast_block_t block_;

public:
  constexpr ast_while_t(ast_block_t &&block)
      : ast_node_t(ast_while_v), block_(std::move(block)) {}

  constexpr ast_block_t &get_block() { return block_; }
  constexpr ast_block_t const &get_block() const { return block_; }

  constexpr ~ast_while_t() = default;
};

// isa implementation

template <typename T> constexpr bool isa(ast_node_t &n);

template <> constexpr bool isa<ast_token_t>(ast_node_t &n) {
  return n.get_kind() == ast_token_v;
}

template <> constexpr bool isa<ast_block_t>(ast_node_t &n) {
  return n.get_kind() == ast_block_v;
}

template <> constexpr bool isa<ast_while_t>(ast_node_t &n) {
  return n.get_kind() == ast_while_v;
}

template <typename T, typename U>
constexpr T *getas(cest::unique_ptr<U> const &p) {
  return static_cast<T *>(p.get());
}

} // namespace brainfuck
