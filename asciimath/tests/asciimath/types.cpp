#include <catch2/catch.hpp>

#include <asciimath/types.hpp>

constexpr bool check_constexpr_validity() {
  asciimath::ast::identifier_t identifier_v;
  asciimath::ast::symbol_t symbol_v;
  asciimath::ast::number_t number_v;
  asciimath::ast::constant_t constant_v;
  asciimath::ast::text_t text_v;
  asciimath::ast::binary_op_t binary_op_v;
  asciimath::ast::binary_expr_t binary_expr_v;
  asciimath::ast::unary_op_t unary_op_v;
  asciimath::ast::unary_expr_t unary_expr_v;
  asciimath::ast::rparen_t rparen_v;
  asciimath::ast::lparen_t lparen_v;
  asciimath::ast::paren_expr_t paren_expr_v;
  asciimath::ast::sub_t sub_v;
  asciimath::ast::super_t super_v;
  asciimath::ast::fraction_t fraction_v;
  asciimath::ast::simp_t simp_v;
  asciimath::ast::expr_t expr_v;

  // Why won't Clang let this compile???
  asciimath::ast::expr_element_t expr_element_v;

  return true;
}

// Related issue:
// https://github.com/llvm/llvm-project/issues/57524
TEST_CASE("AsciiMath type constexpr validity", "[types]") {
  constexpr bool val = check_constexpr_validity();
  REQUIRE(val);
}
