#pragma once

#include <array>

#include <cest/iostream.hpp>
#include <cest/istream.hpp>
#include <cest/ostream.hpp>

#include <brainfog/ast.hpp>

namespace brainfog {

struct program_state_t {
  constexpr program_state_t() noexcept : i(0) {
    for (auto &c : data) {
      c = 0;
    }
  }

  std::array<char, 30000> data;
  std::size_t i;
};

} // namespace brainfog
