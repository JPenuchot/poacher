#pragma once

#include <algorithm>

#include <memory>
#include <string>

#include <brainfuck/ast.hpp>

namespace brainfuck::parser {

/// Parser implementation
namespace impl {

/// Converts a string into a list of BF tokens
constexpr token_vec_t
lex_tokens(std::string const &input) {
  token_vec_t result;
  result.reserve(input.size());
  std::transform(input.begin(), input.end(),
                 std::back_inserter(result),
                 [](auto current_character) {
                   return to_token(current_character);
                 });
  return result;
}

/// Parses BF code until the end of the block (or the
/// end of the formula, ie. parse_end), then returns
/// an iterator to the last parsed token or parse_end
/// if the parser has parsed all the tokens.
constexpr std::tuple<ast_block_t,
                     token_vec_t::const_iterator>
parse_block(token_vec_t::const_iterator parse_begin,
            token_vec_t::const_iterator parse_end) {
  using input_it_t = token_vec_t::const_iterator;

  ast_node_vec_t block_content;

  for (; parse_begin != parse_end; parse_begin++) {
    // While end bracket: return block content and
    // while block end position
    if (*parse_begin == while_end_v) {
      return {std::move(block_content), parse_begin};
    }

    // While begin bracket: recurse,
    // then continue parsing from the end of the block
    else if (*parse_begin == while_begin_v) {
      // Parse while body
      auto [while_block_content, while_block_end] =
          parse_block(parse_begin + 1, parse_end);

      block_content.push_back(
          std::make_unique<ast_while_t>(
              std::move(while_block_content)));

      parse_begin = while_block_end;
    }

    // Any other token that is not a nop instruction:
    // add it to the AST
    else if (*parse_begin != nop_v) {
      block_content.push_back(ast_node_ptr_t(
          std::make_unique<ast_token_t>(
              *parse_begin)));
    }
  }

  return {ast_block_t(std::move(block_content)),
          parse_end};
}

} // namespace impl

/// Driver function for the token parser
constexpr ast_node_ptr_t
parse_ast(std::string const &input) {
  auto const tok = impl::lex_tokens(input);
  auto [block, end] =
      impl::parse_block(tok.cbegin(), tok.cend());
  return std::make_unique<ast_block_t>(
      std::move(block));
}

} // namespace brainfuck::parser
