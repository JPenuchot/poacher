#pragma once

// "flat" backend: flatten the AST into a vector,
// that can itself be turned into an array.
// The transformation is more of a proof of concept since
// this form is *very* close to its original one.

#include <variant>

#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck::flat {

using std::size_t;

struct flat_token_t {
  token_t token;
};

struct flat_block_descriptor_t {
  size_t begin;
  size_t end;
};

struct flat_while_t {
  flat_block_descriptor_t desc;
};

using flat_node_t = std::variant<flat_token_t, flat_while_t>;

using flat_ast_t = cest::vector<flat_node_t>;

namespace detail {

std::size_t get_block_alloc_size(ast_block_t const &b) {
  return b.get_content().size();
}

constexpr void process_block(ast_block_t const &b, flat_ast_t &res) {
  auto beg = res.end();

  // Allocate
  res.resize(res.size() + get_block_alloc_size(b));

  // Parcours des tokens
  std::transform(b.get_content().begin(), b.get_content().end(), beg,
                 [&](ast_node_ptr_t const &p) -> flat_node_t {
                   // While case: recurse
                   if (isa<ast_while_t>(p)) {
                     ast_while_t const *w_ptr = getas<ast_while_t>(p);
                     auto v = flat_while_t{flat_block_descriptor_t{
                         res.size(), res.size() + get_block_alloc_size(
                                                      w_ptr->get_block())}};
                     process_block(w_ptr->get_block(), res);
                     return v;
                   }

                   // Token case: copy
                   if (isa<ast_token_t>(p))
                     return flat_token_t{getas<ast_token_t>(p)->get_token()};

                   return {};
                 });
}

} // namespace detail

constexpr flat_ast_t flatten(ast_node_ptr_t const &p) {
  if (!isa<ast_block_t>(p))
    return {};

  flat_ast_t ast;
  detail::process_block(*getas<ast_block_t>(p), ast);
  return ast;
}

} // namespace brainfuck::flat
