#pragma once

#include <experimental/compiler>
#include <experimental/meta>

#if POACHER_METACLASS == 1

#include <brainfuck/ast.hpp>
#include <brainfuck/program.hpp>

namespace brainfuck {

template <std::experimental::meta::info program_state_r>
consteval std::experimental::meta::info gen_fragment(ast_token_t *n) {
  switch (n->get_token()) {
  case fwd_v:
    return fragment { ++idexpr(program_state_r).i; };

  case bwd_v:
    return fragment { --idexpr(program_state_r).i; };

  case inc_v:
    return fragment {
      ++idexpr(program_state_r).data[idexpr(program_state_r).i];
    };

  case dec_v:
    return fragment {
      --idexpr(program_state_r).data[idexpr(program_state_r).i];
    };

  case put_v:
    return fragment {
      putchar(idexpr(program_state_r).data[idexpr(program_state_r).i]);
    };

  case get_v:
    return fragment {
      idexpr(program_state_r).data[idexpr(program_state_r).i] = getchar();
    };
  default:
    return fragment{};
  }
  return fragment{};
}

consteval std::experimental::meta::info gen_fragment(ast_block_t *n) {
  return fragment{
      // ...
  };
}

consteval std::experimental::meta::info gen_fragment(ast_while_t *n) {
  return fragment{
      // ...
  };
}

} // namespace brainfuck

#endif
