#include <catch2/catch.hpp>

#include <asciimath/parser.hpp>

TEST_CASE("String parsing and whitespace trimming tests", "[parser]") {
  asciimath::parser_t parser("  hello     world");

  REQUIRE(!parser.parse_string("hello"));
  REQUIRE(parser.get_parse_view() == "  hello     world");
  REQUIRE(parser.get_pos() == 0);

  parser.trim_whitespaces();
  REQUIRE(parser.get_parse_view() == "hello     world");
  REQUIRE(parser.get_pos() == 2);

  REQUIRE(parser.parse_string("hello"));
  REQUIRE(parser.get_parse_view() == "     world");
  REQUIRE(parser.get_pos() == 7);

  parser.trim_whitespaces();
  REQUIRE(parser.get_parse_view() == "world");
  REQUIRE(parser.get_pos() == 12);

  REQUIRE(!parser.parse_string("lol"));

  REQUIRE(parser.parse_string("world"));
  REQUIRE(parser.get_parse_view() == "");
  REQUIRE(parser.get_pos() == 17);
}

TEST_CASE("Char parsing", "[parser]") {
  asciimath::parser_t parser("abcd");

  REQUIRE(!parser.parse_char('b'));
  REQUIRE(parser.get_pos() == 0);

  REQUIRE(parser.parse_char('a'));
  REQUIRE(parser.get_pos() == 1);
  REQUIRE(parser.get_parse_view() == "bcd");

  REQUIRE(parser.parse_char('b'));
  REQUIRE(parser.get_pos() == 2);
  REQUIRE(parser.get_parse_view() == "cd");

  REQUIRE(parser.parse_char('c'));
  REQUIRE(parser.get_pos() == 3);
  REQUIRE(parser.get_parse_view() == "d");
}

TEST_CASE("Formula parsing", "[parser]") {
//   std::string_view formula("sum_(i=1)^n i^3=((n(n+1))/2)^2");
  std::string_view formula("2");
  asciimath::parser_t parser(formula);

  asciimath::parser_t::parse_result_t<asciimath::ast::expr_t> result =
      parser.parse_expr();

  REQUIRE(result);
}
