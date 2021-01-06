#pragma once

#if POACHER_METACLASS == 1

#include <experimental/compiler>
#include <experimental/meta>

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck::meta {

/*

template <std::experimental::meta::info s_r>
consteval std::experimental::meta::info gen_fragment(ast_node_ptr_t const &);

template <std::experimental::meta::info s_r>
consteval std::experimental::meta::info gen_fragment(ast_token_t *n) {
  // static_assert(
  //    std::is_same_v<typeof(s_r), brainfuck::program_state_t>,
  //    "s_r: Wrong type for reflected expression.");

  switch (n->get_token()) {
  case fwd_v:
    return fragment { ++valueof(s_r).i; };
  case bwd_v:
    return fragment { --valueof(s_r).i; };
  case inc_v:
    return fragment { ++valueof(s_r).data[valueof(s_r).i]; };
  case dec_v:
    return fragment { --valueof(s_r).data[valueof(s_r).i]; };
  case put_v:
    return fragment { putchar(valueof(s_r).data[valueof(s_r).i]); };
  case get_v:
    return fragment { valueof(s_r).data[valueof(s_r).i] = getchar(); };
  default:
    return fragment {};
  }
  return fragment {};
}

template <std::experimental::meta::info s_r>
consteval std::experimental::meta::info gen_fragment(ast_block_t *n) {
  std::experimental::meta::info f = fragment {};

  for (brainfuck::ast_node_ptr_t const &n : n->get_content()) {
    auto nf = gen_fragment<s_r>(n);
    f = fragment {
      consteval {
        -> %{f};
        -> %{nf};
      };
    };
  }
  return f;
}

template <std::experimental::meta::info s_r>
consteval std::experimental::meta::info gen_fragment(ast_while_t *n) {
  auto f = fragment {
    while (valueof(s_r).data[valueof(s_r).i]) {
      consteval->gen_fragment<s_r>(n->get_block());
    }
  };

  return f;
}

template <std::experimental::meta::info s_r>
consteval std::experimental::meta::info gen_fragment(ast_node_ptr_t const &n) {
  switch (n->get_kind()) {
  case ast_token_v:
    return gen_fragment<s_r>(getas<ast_token_t>(n));
  case ast_block_v:
    return gen_fragment<s_r>(getas<ast_block_t>(n));
  case ast_while_v:
    return gen_fragment<s_r>(getas<ast_while_t>(n));
  }
  return fragment{};
}

*/

} // namespace brainfuck::meta

#endif
