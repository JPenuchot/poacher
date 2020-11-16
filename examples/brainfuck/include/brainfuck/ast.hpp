#pragma once

#include <cest/memory.hpp>
#include <cest/vector.hpp>

namespace brainfuck {

//------------------------------------------------------------------------------
// TOKEN TYPE

enum token_t : char
{
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

constexpr enum token_t
to_token(char c)
{
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

enum ast_node_kind_t : std::uint8_t
{
  token_node_v,
  block_node_v,
  while_node_v,
};

class ast_node_t
{
private:
  ast_node_kind_t kind_;

protected:
  constexpr ast_node_t(ast_node_kind_t kind)
    : kind_(kind){};

public:
  constexpr ast_node_kind_t get_kind() const { return kind_; }
};

class token_node_t : public ast_node_t
{
  token_t token_;

public:
  constexpr token_node_t(token_t t)
    : ast_node_t(token_node_v)
    , token_(t)
  {}
};

class block_node_t : public ast_node_t
{
public:
  using node_ptr_t = cest::unique_ptr<ast_node_t>;

private:
  cest::vector<node_ptr_t> content_;

public:
  constexpr block_node_t()
    : ast_node_t(block_node_v)
  {}

  constexpr block_node_t(cest::vector<node_ptr_t>&& v)
    : block_node_t()
  {
    content_ = std::move(v);
  }

  constexpr cest::vector<node_ptr_t>& get_content() { return content_; }
  constexpr cest::vector<node_ptr_t> const& get_content() const
  {
    return content_;
  }
};

class while_node_t : public ast_node_t
{
  block_node_t block_;

public:
  constexpr while_node_t(cest::vector<cest::unique_ptr<ast_node_t>>&& v)
    : ast_node_t(while_node_v)
    , block_(std::move(v))
  {}

  constexpr block_node_t& get_block() { return block_; }
  constexpr block_node_t const& get_block() const { return block_; }
};

// isa implementation

template<typename T>
bool
isa(ast_node_t* n);

template<>
bool
isa<token_node_t>(ast_node_t* n)
{
  return n->get_kind() == token_node_v;
}

template<>
bool
isa<block_node_t>(ast_node_t* n)
{
  return n->get_kind() == block_node_v;
}

template<>
bool
isa<while_node_t>(ast_node_t* n)
{
  return n->get_kind() == while_node_v;
}

// getas implementation

template<typename T>
T*
getas(ast_node_t* n);

template<>
token_node_t*
getas<token_node_t>(ast_node_t* n)
{
  return isa<token_node_t>(n) ? reinterpret_cast<token_node_t*>(n) : nullptr;
}

template<>
block_node_t*
getas<block_node_t>(ast_node_t* n)
{
  return isa<block_node_t>(n) ? reinterpret_cast<block_node_t*>(n) : nullptr;
}

template<>
while_node_t*
getas<while_node_t>(ast_node_t* n)
{
  return isa<while_node_t>(n) ? reinterpret_cast<while_node_t*>(n) : nullptr;
}

} // namespace brainfuck
