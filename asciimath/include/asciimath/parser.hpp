#pragma once

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <locale>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <asciimath/types.hpp>
#include <unistd.h>

namespace asciimath {

//==============================================================================
//
// PARSING STRUCTURES
//

/// Manages parsing state by keeping track of the formula and its parsing state
class parser_t {
  /// Formula string container
  std::string _phrase;

  /// Current parsing position
  std::size_t _pos;

public:
  constexpr parser_t(std::string formula)
      : _phrase(std::move(formula)), _pos(0) {}

  /// Get current parsing position
  constexpr std::size_t get_pos() const { return _pos; }

  /// Set parsing position
  constexpr void set_pos(std::size_t v) { _pos = v; }

  /// Get a string_view of the AsciiMath formula
  constexpr std::string_view get_phrase() const { return _phrase; }

  /// Get a string view of the remaining text to parse
  constexpr std::string_view get_parse_view() const {
    return {_phrase.begin() + _pos, _phrase.end()};
  }

  /// Returns remaining text size
  constexpr std::size_t size() const { return _phrase.size() - _pos; }

  /// Increase position counter after a token was consumed
  /// then return the new position
  constexpr std::size_t consume(std::size_t count) {
    _pos += count;
    return _pos;
  }

  /// Trim whitespaces
  constexpr void trim_whitespaces() {
    _pos = std::find_if_not(_phrase.begin() + _pos, _phrase.end(),
                            [](std::string_view::value_type c) {
                              return std::isblank(c, std::locale::classic());
                            }) -
           _phrase.begin();
  }

  /// Wraps the result of a parsing function
  template <typename T> class parse_result_t {
    std::optional<T> _value;

    /// Begin position of the token in the formula string
    std::size_t _begin_pos;
    /// End position of the token in the formula string
    std::size_t _end_pos;

  public:
    /// Construct a parse result
    constexpr parse_result_t(T value, std::size_t begin_pos,
                             std::size_t end_pos)
        : _value(std::move(value)), _begin_pos(begin_pos), _end_pos(end_pos) {}

    /// Construct an empty parse result
    constexpr parse_result_t() : _value(std::nullopt) {}

    /// Return true if there is a result, otherwise false
    constexpr operator bool() const { return bool(_value); }

    /// Extract token into a string_view
    constexpr std::string_view get_view(parser_t const &pm) {
      return {pm.get_phrase().begin() + _begin_pos,
              pm.get_phrase().begin() + _end_pos};
    }

    /// Get begin position of the token in the formula string
    constexpr std::size_t get_begin_pos() const { return _begin_pos; }

    /// Get end position of the token in the formula string
    constexpr std::size_t get_end_pos() const { return _end_pos; }

    /// Returns reference to value opt
    constexpr std::optional<T> &get_opt() { return _value; }

    /// Returns const reference to value opt
    constexpr std::optional<T> const &get_opt() const { return _value; }
  };

  //============================================================================
  //
  // PARSING FUNCTIONS
  //

  //----------------------------------------------------------------------------
  // identifier = [A-z]

  constexpr parse_result_t<identifier_t> parse_identifier() {
    trim_whitespaces();
    std::string_view sv = get_parse_view();

    // Beginning of the identifier
    std::string_view::const_iterator begin = sv.begin();

    // Find first char that isn't a letter
    std::string_view::const_iterator end = std::find_if_not(
        sv.begin(), sv.end(), [](std::string_view::value_type c) -> bool {
          return std::isalpha(c, std::locale::classic());
        });

    // Empty identifier? Empty result.
    if (end == begin) {
      return {};
    }

    // Non-empty identifier
    return {identifier_t{{begin, end}}, get_pos(), consume(end - begin)};
  }

  //----------------------------------------------------------------------------
  // symbol = /* any string in the symbol table */

  constexpr parse_result_t<symbol_t> parse_symbol() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // number = '-'? [0-9]+ ( '.' [0-9]+ )?

  constexpr parse_result_t<number_t> parse_number() {
    trim_whitespaces();
    std::string_view sv = get_parse_view();

    std::size_t consumed = 0;
    int sign = 1;
    int val = 0;

    // Reading sign
    if (sv.front() == '-') {
      sign = -1;
      sv.remove_prefix(1);
      consumed++;
    }

    // First digit is mandatory
    if (!std::isdigit(sv.front(), std::locale::classic())) {
      return {};
    }

    // Accumulating into an int...
    while (std::isdigit(sv.front(), std::locale::classic())) {
      val *= 10;
      val += sv.front() - '0';
      sv.remove_prefix(1);
      consumed++;
    }

    return {{val * sign}, get_pos(), consume(consumed)};
  }

  //----------------------------------------------------------------------------
  // constant = number | symbol | identifier

  constexpr parse_result_t<constant_t> parse_constant() {
    // Number parsing attempt
    if (parse_result_t<number_t> number = parse_number(); number) {
      return {
          {*number.get_opt()}, number.get_begin_pos(), number.get_end_pos()};
    }

    // Symbol parsing attempt
    if (parse_result_t<symbol_t> symbol = parse_symbol(); symbol) {
      return {
          {*symbol.get_opt()}, symbol.get_begin_pos(), symbol.get_end_pos()};
    }

    // Identifier parsing attempt
    if (parse_result_t<identifier_t> identifier = parse_identifier();
        identifier) {
      return {{*identifier.get_opt()},
              identifier.get_begin_pos(),
              identifier.get_end_pos()};
    }

    // Failure
    return {};
  }

  //----------------------------------------------------------------------------
  // text = '"' [^"]* '"'

  constexpr parse_result_t<text_t> parse_text() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // binary_op = 'frac' | 'root' | 'stackrel'

  constexpr parse_result_t<binary_op_t> parse_binary_op() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // binary_expr = binary_op simp simp

  constexpr parse_result_t<binary_expr_t> parse_binary_expr() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Binop parsing attempt
    parse_result_t<binary_op_t> binop_pr = parse_binary_op();
    if (!binop_pr) {
      return {};
    }

    // Simp parsing attempt
    parse_result_t<simp_t> simp_pr_1 = parse_simp();
    if (!simp_pr_1) {
      return {};
    }

    // Simp parsing attempt
    parse_result_t<simp_t> simp_pr_2 = parse_simp();
    if (!simp_pr_2) {
      return {};
    }

    return {{*binop_pr.get_opt(),
             make_simp_ptr(std::move(*simp_pr_1.get_opt())),
             make_simp_ptr(std::move(*simp_pr_2.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // unary_op = 'sqrt' | 'text'

  constexpr parse_result_t<unary_op_t> parse_unary_op() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // unary_expr = unary_op simp

  constexpr parse_result_t<unary_expr_t> parse_unary_expr() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Unary op parsing attempt
    parse_result_t<unary_op_t> op_pr = parse_unary_op();
    if (!op_pr) {
      return {};
    }

    // Simp parsing attempt
    parse_result_t<simp_t> simp_pr = parse_simp();
    if (!simp_pr) {
      return {};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // rparen = ')' | ']' | '}' | ':)' | ':}'

  constexpr parse_result_t<rparen_t> parse_rparen() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // lparen = '(' | '[' | '{' | '(:' | '{:'

  constexpr parse_result_t<lparen_t> parse_lparen() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // paren_expr = lparen expr rparen

  constexpr parse_result_t<paren_expr_t> parse_paren_expr() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    parse_result_t<lparen_t> lparen_pr = parse_lparen();
    if (!lparen_pr) {
      return {};
    }

    parse_result_t<expr_t> expr_pr = parse_expr();
    if (!expr_pr) {
      return {};
    }

    parse_result_t<rparen_t> rparen_pr = parse_rparen();
    if (!rparen_pr) {
      return {};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // sub =  '_' simp super?

  constexpr parse_result_t<sub_t> parse_sub() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // super = '^' simp

  constexpr parse_result_t<super_t> parse_super() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // fraction = '/' simp

  constexpr parse_result_t<fraction_t> parse_fraction() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // simp = constant | paren_expr | unary_expr | binary_expr | text

  constexpr parse_result_t<simp_t> parse_simp() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Constant parsing attempt
    if (parse_result_t<constant_t> pr = parse_constant(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Paren expr parsing attempt
    if (parse_result_t<paren_expr_t> pr = parse_paren_expr(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Unary expr parsing attempt
    if (parse_result_t<unary_expr_t> pr = parse_unary_expr(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Binary expr parsing attempt
    if (parse_result_t<binary_expr_t> pr = parse_binary_expr(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Text parsing attempt
    if (parse_result_t<text_t> pr = parse_text(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // expr = ( simp ( fraction | sub | super ) )+

  constexpr parse_result_t<expr_t> parse_expr() {
    // TODO
    return {};
  }
};

} // namespace asciimath
