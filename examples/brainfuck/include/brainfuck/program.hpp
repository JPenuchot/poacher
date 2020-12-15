#pragma once

#include <array>
#include <iostream>

#include <brainfuck/ast.hpp>

namespace brainfuck {

struct program_state_t {
  std::array<char, 30000> data;
  std::size_t i;
};

} // namespace brainfuck
