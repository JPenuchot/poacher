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

constexpr auto parse_ast(token_vec_t::iterator begin, token_vec_t::iterator end)
    -> block_node_t {
  using input_it_t = token_vec_t::iterator;

  block_node_t block;
  ast_node_vec_t &ast_vec = block.get_content();

  while (begin != end) {
    // parse tokens until while block beginning
    for (; begin != end; begin++)
      if (*begin != nop_v)
        ast_vec.push_back(ast_node_ptr_t(new token_node_t(*begin)));

    if (begin != end) {

      begin++; // enter while block

      // find block end
      input_it_t w_end = begin;
      for (unsigned l = 1;
           w_end != end && (l += (*w_end == whb_v) - (*w_end == whe_v));
           w_end++)
        ;

      // Check for unmatched while begin
      if (w_end == end) {
        cest::cout << "Unmatched while begin\n";
        return block;
      }

      // parse while content
      ast_vec.push_back(
          ast_node_ptr_t(new block_node_t(parse_ast(begin, w_end))));

      begin = w_end; // commit

      begin++; // exit block
    }
  }

  return block;
}

constexpr auto parse_ast(cest::string const &input) {
  auto tok = lex_tokens(input);
  return parse_ast(tok.begin(), tok.end());
}

} // namespace brainfuck
