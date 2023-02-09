#include <algorithm>
#include <array>
#include <bits/utility.h>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <vector>

#include <asciimath/asciimath.hpp>

template <auto Fun> constexpr auto eval_as_tuple() {
  constexpr std::size_t S = Fun().size();

  return []<std::size_t... IndexList>(std::index_sequence<IndexList...>) {
    return std::make_tuple(Fun()[IndexList]...);
    // return std::make_tuple([]() constexpr { return Fun()[IndexList]; }...);
  }
  (std::make_index_sequence<S>{});
}

template <auto Fun> constexpr auto eval_as_array() {
  using T = typename std::decay_t<decltype(Fun())>::value_type;
  constexpr std::size_t S = Fun().size();

  std::array<T, S> res;
  std::ranges::copy(Fun(), res.begin());
  return res;
}

constexpr std::vector<std::size_t> gen_vec() { return {0, 1, 2, 3}; }
constexpr auto tuple = eval_as_tuple<&gen_vec>();
constexpr auto array = eval_as_array<&gen_vec>();

int main(int argc, char *argv[]) {}
