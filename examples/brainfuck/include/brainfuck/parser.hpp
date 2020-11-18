#pragma once

#include <algorithm>

#include <cest/memory.hpp>
#include <cest/string.hpp>
#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck {

//------------------------------------------------------------------------------
// PARSING

constexpr token_vec_t lex_tokens(cest::string const &input) {
  token_vec_t vec;
  vec.reserve(input.size());
  std::transform(input.begin(), input.end(), std::back_inserter(vec),
                 [](auto c) { return to_token(c); });
  return vec;
}

constexpr auto consume_tokens(token_vec_t::iterator begin,
                              token_vec_t::iterator end,
                              std::back_insert_iterator<ast_node_vec_t> out)
    -> token_vec_t::iterator {
  for (; begin != end; begin++, out++)
    if (*begin != nop_v)
      *out = ast_node_ptr_t(new token_node_t(*begin));
  return begin;
}

constexpr auto parse_ast(token_vec_t::iterator begin, token_vec_t::iterator end)
    -> block_node_t {
  using input_it_t = token_vec_t::iterator;

  block_node_t block;
  ast_node_vec_t &ast_vec = block.get_content();

  auto parse_while = [&](input_it_t b, input_it_t e) -> input_it_t {
    auto find_while_end = [&](input_it_t b, input_it_t e) -> input_it_t {
      unsigned l = 0;
      do
        l += (*b == whb_v) - (*b == whe_v);
      while (++b != e && l != 0);
      return b;
    };

    input_it_t w_end = find_while_end(b, e);
    ast_vec.push_back(ast_node_ptr_t(new block_node_t(parse_ast(b, w_end))));

    return ++b;
  };

  // Core algorithm

  while (begin != end) {
    begin = consume_tokens(begin, std::find(begin, end, whb_v),
                           std::back_inserter(ast_vec));
    if (begin != end)
      begin = parse_while(begin, end);
  }

  return block;
}

} // namespace brainfuck
