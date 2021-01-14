#pragma once

#include <variant>

#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck::flat {

using std::size_t;

struct flat_token_t {
  token_t token;
};

struct flat_block_t {
  size_t begin;
  size_t end;
};

struct flat_while_t {
  size_t block_descriptor;
};

using flat_node_t = std::variant<flat_token_t, flat_block_t, flat_while_t>;

using flat_ast_t = cest::vector<flat_node_t>;

namespace detail {

auto &process(ast_token_t *p, flat_ast_t &res) { return res; }

template <typename OutputIt>
auto &process(ast_block_t *p, flat_ast_t &res, OutputIt out) {

  return res;
}

auto &process(ast_while_t *p, flat_ast_t &res) {
  auto const &b = p->get_block();

  // Allocate

  auto token_begin = res.end();

  res.resize(res.size() + b.get_content().size());

  // Parcours des tokens

  std::transform(b.get_content().begin(), b.get_content().end(), token_begin,
                 [&](ast_node_ptr_t const &p) -> flat_node_t {
                  return {};
                });

  // Return

  return res;
}

} // namespace detail

flat_ast_t flatten(ast_node_ptr_t const &p) { return {}; }

} // namespace brainfuck::flat
