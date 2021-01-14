#pragma once

#include <variant>

#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck::flat {

using std::size_t;

struct flat_token_t {
  token_t token;
};

struct block_descriptor_t {
  size_t begin;
  size_t end;
};

struct flat_while_t {
  block_descriptor_t desc;
};

using flat_node_t = std::variant<flat_token_t, flat_while_t>;

using flat_ast_t = cest::vector<flat_node_t>;

namespace detail {

constexpr auto &process_block(ast_block_t *b_ptr, flat_ast_t &res) {
  // Allocate
  auto token_begin = res.end();
  res.resize(res.size() + b_ptr->get_content().size());

  // Parcours des tokens
  std::transform(b_ptr->get_content().begin(), b_ptr->get_content().end(),
                 token_begin, [&](ast_node_ptr_t const &p) -> flat_node_t {
                   if (isa<ast_token_t>(p)) {
                     return flat_token_t{getas<ast_token_t>(p)->get_token()};
                   }

                   if (isa<ast_while_t>(p)) {
                     ast_while_t const *w_ptr = getas<ast_while_t>(p);
                     return flat_while_t{block_descriptor_t{
                         res.size(),
                         res.size() + w_ptr->get_block().get_content().size()}};
                   }

                   if (isa<ast_block_t>(p)) {
                     // Should be impossible
                     return {};
                   }

                   // If not... Then what are you ?!
                   return {};
                 });

  // Return
  return res;
}

} // namespace detail

constexpr flat_ast_t flatten(ast_node_ptr_t const &p) {
  if (!isa<ast_block_t>(p))
    return {};

  flat_ast_t ast;
  detail::process_block(getas<ast_block_t>(p), ast);
  return std::move(ast);
}

} // namespace brainfuck::flat
