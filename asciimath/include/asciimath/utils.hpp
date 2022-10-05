#pragma once

#include <experimental/source_location>

#include <fmt/core.h>

#include <asciimath/types.hpp>

namespace asciimath::utils {

#ifdef NDEBUG
constexpr bool debug = false;
#else
constexpr bool debug = true;
#endif

/// Extracts typename from a type
template <typename T> inline constexpr std::string_view type_as_string_view;

// Temporary specialization prefix macro, undefined after the specializations
#define TMP_SORT_DEF_PREFIX                                                    \
  template <> inline constexpr std::string_view type_as_string_view

TMP_SORT_DEF_PREFIX<ast::identifier_t> = "identifier_t";
TMP_SORT_DEF_PREFIX<ast::symbol_t> = "symbol_t";
TMP_SORT_DEF_PREFIX<ast::number_t> = "number_t";
TMP_SORT_DEF_PREFIX<ast::constant_t> = "constant_t";
TMP_SORT_DEF_PREFIX<ast::text_t> = "text_t";
TMP_SORT_DEF_PREFIX<ast::binary_op_t> = "binary_op_t";
TMP_SORT_DEF_PREFIX<ast::binary_expr_t> = "binary_expr_t";
TMP_SORT_DEF_PREFIX<ast::unary_op_t> = "unary_op_t";
TMP_SORT_DEF_PREFIX<ast::unary_expr_t> = "unary_expr_t";
TMP_SORT_DEF_PREFIX<ast::rparen_t> = "rparen_t";
TMP_SORT_DEF_PREFIX<ast::lparen_t> = "lparen_t";
TMP_SORT_DEF_PREFIX<ast::paren_expr_t> = "paren_expr_t";
TMP_SORT_DEF_PREFIX<ast::sub_t> = "sub_t";
TMP_SORT_DEF_PREFIX<ast::super_t> = "super_t";
TMP_SORT_DEF_PREFIX<ast::fraction_t> = "fraction_t";
TMP_SORT_DEF_PREFIX<ast::simp_t> = "simp_t";
TMP_SORT_DEF_PREFIX<ast::expr_t> = "expr_t";

#undef TMP_SORT_DEF_PREFIX

/// Emits a trace in the console.
constexpr void trace(std::string_view message = {},
                     std::experimental::source_location sloc =
                         std::experimental::source_location::current()) {
  if (!std::is_constant_evaluated()) {
    if constexpr (debug) {
      if (message.empty()) {
        fmt::print("[TRACE INFO] {} - {}:{} {}\n", sloc.file_name(),
                   sloc.line(), sloc.column(), sloc.function_name());
      } else {
        fmt::print("[TRACE INFO] {} - {}:{} {} - {}\n", sloc.file_name(),
                   sloc.line(), sloc.column(), sloc.function_name(), message);
      }
    }
  }
}

/// Returns true if c is a digit, otherwise false.
constexpr bool is_digit(char c) { return c >= '0' && c <= '9'; }

/// Returns true if c is blank, otherwise false.
constexpr bool is_blank(char c) { return c == '\n' || c == ' ' || c == '\n'; }

/// Returns true if c is an alphanumeric character, otherwise false.
constexpr bool is_alpha(char c) {
  return is_digit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//==============================================================================
// Print functions

constexpr std::string print(ast::identifier_t const &identifier,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::symbol_t const &symbol, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::number_t const &number, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::constant_t const &constant,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::text_t const &text, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::binary_op_t const &binary_op,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::binary_expr_t const &binary_expr,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::unary_op_t const &unary_op,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::unary_expr_t const &unary_expr,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::rparen_t const &rparen, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::lparen_t const &lparen, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::paren_expr_t const &paren_expr,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::sub_t const &sub, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::super_t const &super, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::fraction_t const &fraction,
                            std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::simp_t const &simp, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

constexpr std::string print(ast::expr_t const &expr, std::size_t indent = 0,
                            std::string_view indent_string = "  ") {
  return {};
}

} // namespace asciimath::utils
