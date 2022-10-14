#pragma once

#include <array>
#include <iostream>

#include <brainfuck/ast.hpp>

namespace brainfuck {

struct program_state_t {
  constexpr program_state_t() noexcept : i(0) {
    for (auto &c : data) {
      c = 0;
    }
  }

  std::array<char, 30000> data;
  std::size_t i;
};

} // namespace brainfuck
