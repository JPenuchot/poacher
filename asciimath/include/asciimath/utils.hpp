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

} // namespace asciimath::utils
