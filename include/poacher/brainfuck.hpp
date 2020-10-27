#pragma once

namespace poacher::brainfuck {

enum node_type_v
{
  token_v,
  while_v,
};

struct ast_node_t
{
  node_type_v type;

  constexpr ast_node_t(node_type_v type_)
    : type(type_)
  {}

  constexpr virtual ~ast_node_t() {}
};

enum token_v : char
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
  nop_v, // error
};

constexpr enum token_v
to_token(char c)
{
  switch (c) {
    case fwd_v: return fwd_v;
    case bwd_v: return bwd_v;
    case inc_v: return inc_v;
    case dec_v: return dec_v;
    case put_v: return put_v;
    case get_v: return get_v;
    case whb_v: return whb_v;
    case whe_v: return whe_v;
  }
  return nop_v;
}

struct token_node_t : ast_node_t
{
  constexpr token_node_t()
    : ast_node_t{ node_type_v::token_v }
  {}
};

struct while_node_t : ast_node_t
{
  constexpr while_node_t()
    : ast_node_t{ node_type_v::while_v }
  {}
};



} // namespace poacher
