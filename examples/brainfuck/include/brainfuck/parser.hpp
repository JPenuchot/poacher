#pragma once

#include <algorithm>

#include <cest/memory.hpp>
#include <cest/string.hpp>
#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck {

namespace detail {
//------------------------------------------------------------------------------
// PARSING

constexpr token_vec_t lex_tokens(cest::string const &input) {
  token_vec_t vec;
  vec.reserve(input.size());
  std::transform(input.begin(), input.end(), std::back_inserter(vec),
                 [](auto c) { return to_token(c); });
  return vec;
}

constexpr ast_block_t parse_block(token_vec_t::const_iterator begin,
                                  token_vec_t::const_iterator end) {
  using input_it_t = token_vec_t::const_iterator;

  ast_node_vec_t ast_vec;

  for (; begin != end; begin++) {
    // parse tokens until while block beginning
    if (*begin == nop_v)
      continue;

    if (*begin == whb_v) {
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
        return ast_block_t(std::move(ast_vec));
      }

      // parse while content
      ast_vec.push_back(
          cest::make_unique<ast_while_t>(parse_block(begin, w_end)));

      begin = w_end; // commit
      continue;      // exit while
    }

    ast_vec.push_back(ast_node_ptr_t(cest::make_unique<ast_token_t>(*begin)));
  }

  return ast_block_t(std::move(ast_vec));
}

} // namespace detail

constexpr ast_node_ptr_t parse_ast(cest::string const &input) {
  auto const tok = detail::lex_tokens(input);
  return cest::make_unique<ast_block_t>(
      detail::parse_block(tok.cbegin(), tok.cend()));
}

} // namespace brainfuck
