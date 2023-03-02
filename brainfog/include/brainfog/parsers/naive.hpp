#pragma once

#include <algorithm>

#include <cest/memory.hpp>
#include <cest/string.hpp>

#include <brainfog/ast.hpp>

namespace brainfog::naive_parser {

namespace detail {
//------------------------------------------------------------------------------
// PARSING

/// Converts a string into a list of BF tokens
constexpr token_vec_t lex_tokens(cest::string const &input) {
  token_vec_t result;
  result.reserve(input.size());
  std::transform(
      input.begin(), input.end(), std::back_inserter(result),
      [](auto current_character) { return to_token(current_character); });
  return result;
}

/// Parses a block of BF code
constexpr ast_block_t parse_block(token_vec_t::const_iterator parse_begin,
                                  token_vec_t::const_iterator parse_end) {
  using input_it_t = token_vec_t::const_iterator;

  ast_node_vec_t ast_vec;

  for (; parse_begin != parse_end; parse_begin++) {
    // parse tokens until while block beginning
    if (*parse_begin == nop_v)
      continue;

    // NOTE: While block parsing could be largely improved by adding a specific
    // function for while block parsing, that finds the while block end *and*
    // parses token at the same time.
    if (*parse_begin == while_begin_v) {
      parse_begin++; // enter while block

      // Find block end
      input_it_t while_end = parse_begin;
      for (unsigned block_depth = 1;
           while_end != parse_end &&
           (block_depth +=
            (*while_end == while_begin_v) - (*while_end == while_end_v)) > 0;
           while_end++)
        ;

      // Check for unmatched while begin
      if (while_end == parse_end) {
        cest::cout << "Unmatched while begin\n";
        std::exit(1);
      }

      // parse while content
      ast_vec.push_back(
          cest::make_unique<ast_while_t>(parse_block(parse_begin, while_end)));

      parse_begin = while_end; // commit
      continue;                // exit while
    }

    ast_vec.push_back(
        ast_node_ptr_t(cest::make_unique<ast_token_t>(*parse_begin)));
  }

  return ast_block_t(std::move(ast_vec));
}

} // namespace detail

constexpr ast_node_ptr_t parse_ast(cest::string const &input) {
  auto const tok = detail::lex_tokens(input);
  return cest::make_unique<ast_block_t>(
      detail::parse_block(tok.cbegin(), tok.cend()));
}

} // namespace brainfog::naive_parser
