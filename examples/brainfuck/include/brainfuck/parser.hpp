#pragma once

#include <algorithm>

#include <cest/memory.hpp>
#include <cest/string.hpp>
#include <cest/vector.hpp>

#include <brainfuck/ast.hpp>

namespace brainfuck {

//------------------------------------------------------------------------------
// PARSING

constexpr cest::vector<token_t>
lex_tokens(cest::string const& input)
{
  cest::vector<token_t> vec;
  vec.reserve(input.size());
  std::transform(input.begin(),
                 input.end(),
                 std::back_inserter(vec),
                 [](auto c) { return to_token(c); });
  return vec;
}

template<typename InputIt, typename OutputIt>
auto
consume_tokens(InputIt begin, InputIt end, OutputIt out) -> decltype(begin)
{
  std::copy_if(begin, end, out, [&](auto const& e) { return e != nop_v; });
  return begin;
}

template<typename InputIt>
constexpr block_node_t
parse_ast(InputIt begin, InputIt end) // cest::vector<token_t>
{
  using node_ptr_t = cest::unique_ptr<ast_node_t>;

  cest::unique_ptr<block_node_t> b_ptr(new block_node_t());
  cest::vector<node_ptr_t>& ast_vec = (*b_ptr).get_content();

  // Helper lambdas

  auto find_while_begin = [&](InputIt b, InputIt e) -> InputIt {
    return std::find(b, e, whb_v);
  };

  auto find_while_end = [&](InputIt b, InputIt e) -> InputIt {
    unsigned l = 0;
    do
      l += (*b == whb_v) - (*b == whe_v);
    while (l != 0 && ++b != e);
    return b;
  };

  auto parse_while = [&](InputIt b, InputIt e) -> InputIt {
    auto w_end = find_while_end(b, e);
    auto ast = parse_ast(b, w_end);
    return b;
  };

  // Core algorithm

  while (begin != end) {
    begin = consume_tokens(begin, find_while_begin(begin, end));
    if (begin != end)
      begin = parse_while(begin, end);
  }

  return {};
}

} // namespace brainfuck
