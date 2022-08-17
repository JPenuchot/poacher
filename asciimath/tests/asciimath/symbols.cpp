#include <catch2/catch.hpp>

#include <algorithm>

#include <asciimath/symbols.hpp>

/// Check for symbols in the array being of the right kind
template <asciimath::symbols::symbol_kind_t Kind>
constexpr bool valid_symbols() {
  auto const &sym = asciimath::symbols::symbols_by_kind<Kind>;
  return std::all_of(sym.begin(), sym.end(),
                     [](asciimath::symbols::symbol_def_t const &s) -> bool {
                       return s.kind == Kind;
                     });
}

/// Check for symbol array not being empty
template <asciimath::symbols::symbol_kind_t Kind>
constexpr bool non_empty_symbols() {
  return !asciimath::symbols::symbols_by_kind<Kind>.empty();
}

/// Check for symbol array being sorted by symbol size
template <asciimath::symbols::symbol_kind_t Kind>
constexpr bool sorted_symbols() {
  auto const &sym = asciimath::symbols::symbols_by_kind<Kind>;
  return std::is_sorted(sym.begin(), sym.end(),
                        [](asciimath::symbols::symbol_def_t const &a,
                           asciimath::symbols::symbol_def_t const &b) -> bool {
                          return a.input.size() > b.input.size();
                        });
}

TEST_CASE("Symbol arrays validity", "[symbols]") {
  REQUIRE(valid_symbols<asciimath::symbols::binary_v>());
  REQUIRE(valid_symbols<asciimath::symbols::const_v>());
  REQUIRE(valid_symbols<asciimath::symbols::definition_v>());
  REQUIRE(valid_symbols<asciimath::symbols::infix_v>());
  REQUIRE(valid_symbols<asciimath::symbols::leftbracket_v>());
  REQUIRE(valid_symbols<asciimath::symbols::leftright_v>());
  REQUIRE(valid_symbols<asciimath::symbols::rightbracket_v>());
  REQUIRE(valid_symbols<asciimath::symbols::space_v>());
  REQUIRE(valid_symbols<asciimath::symbols::text_v>());
  REQUIRE(valid_symbols<asciimath::symbols::unary_v>());
  REQUIRE(valid_symbols<asciimath::symbols::unaryunderover_v>());
  REQUIRE(valid_symbols<asciimath::symbols::underover_v>());

  REQUIRE(sorted_symbols<asciimath::symbols::binary_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::const_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::definition_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::infix_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::leftbracket_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::leftright_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::rightbracket_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::space_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::text_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::unary_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::unaryunderover_v>());
  REQUIRE(sorted_symbols<asciimath::symbols::underover_v>());

  REQUIRE(non_empty_symbols<asciimath::symbols::binary_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::const_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::definition_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::infix_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::leftbracket_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::leftright_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::rightbracket_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::space_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::text_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::unary_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::unaryunderover_v>());
  REQUIRE(non_empty_symbols<asciimath::symbols::underover_v>());
}
