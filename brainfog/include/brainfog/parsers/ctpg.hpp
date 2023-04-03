#pragma once
/*
#include "brainfog/ast.hpp"

#include <cest/memory.hpp>
#include <ctpg/buffers.hpp>
#include <ctpg/ctpg.hpp>

namespace brainfog::ctpg_parser {

constexpr ast_node_ptr_t parse_ast(char const *program_string) {
  // Non-terminal symbols
  ctpg::nterm<cest::unique_ptr<ast_token_t>> token_nterm("token");
  ctpg::nterm<cest::unique_ptr<ast_while_t>> while_nterm("while");
  ctpg::nterm<cest::unique_ptr<ast_block_t>> block_nterm("block");

  // Instruction tokens
  ctpg::char_term fwd_term('>');
  ctpg::char_term bwd_term('<');
  ctpg::char_term inc_term('+');
  ctpg::char_term dec_term('-');
  ctpg::char_term put_term('.');
  ctpg::char_term get_term(',');

  // While begin/end tokens
  ctpg::char_term whb_term('[');
  ctpg::char_term whe_term(']');

  ctpg::parser p(
      block_nterm,
      ctpg::terms(fwd_term, bwd_term, inc_term, dec_term, put_term, get_term,
                  whb_term, whe_term),
      ctpg::nterms(token_nterm, while_nterm, block_nterm),
      ctpg::rules(

          // Instruction rules
          token_nterm(fwd_term) >= [](char) -> cest::unique_ptr<ast_token_t> {
            return cest::make_unique<ast_token_t>(fwd_v);
          },
          token_nterm(bwd_term) >= [](char) -> cest::unique_ptr<ast_token_t> {
            return cest::make_unique<ast_token_t>(bwd_v);
          },
          token_nterm(inc_term) >= [](char) -> cest::unique_ptr<ast_token_t> {
            return cest::make_unique<ast_token_t>(inc_v);
          },
          token_nterm(dec_term) >= [](char) -> cest::unique_ptr<ast_token_t> {
            return cest::make_unique<ast_token_t>(dec_v);
          },
          token_nterm(put_term) >= [](char) -> cest::unique_ptr<ast_token_t> {
            return cest::make_unique<ast_token_t>(put_v);
          },
          token_nterm(get_term) >= [](char) -> cest::unique_ptr<ast_token_t> {
            return cest::make_unique<ast_token_t>(get_v);
          },

          // Block rules
          block_nterm(token_nterm) >= [](cest::unique_ptr<ast_token_t> &&t)
              -> cest::unique_ptr<ast_block_t> {
            ast_node_vec_t v;
            v.emplace_back(std::move(t));
            return cest::make_unique<ast_block_t>(std::move(v));
          },

          block_nterm(while_nterm) >= [](cest::unique_ptr<ast_while_t> &&w)
              -> cest::unique_ptr<ast_block_t> {
            ast_node_vec_t v;
            v.emplace_back(std::move(w));
            return cest::make_unique<ast_block_t>(std::move(v));
          },

          block_nterm(block_nterm, token_nterm) >=
              [](cest::unique_ptr<ast_block_t> &&b,
                 cest::unique_ptr<ast_token_t> &&i)
              -> cest::unique_ptr<ast_block_t> {
            b->get_content().push_back(std::move(i));
            return b;
          },

          block_nterm(block_nterm, while_nterm) >=
              [](cest::unique_ptr<ast_block_t> &&b,
                 cest::unique_ptr<ast_while_t> &&w)
              -> cest::unique_ptr<ast_block_t> {
            b->get_content().push_back(std::move(w));
            return b;
          },

          // While block rule
          while_nterm(whb_term, block_nterm, whe_term) >=
              [](char, cest::unique_ptr<ast_block_t> &&b,
                 char) -> cest::unique_ptr<ast_while_t> {
            return cest::make_unique<ast_while_t>(std::move(*b));
          }));

  ctpg::buffers::string_buffer buff(program_string);

  std::optional<int> val;
  return cest::make_unique<ast_block_t>(ast_node_vec_t{});
}

} // namespace brainfog::ctpg_parser*/
