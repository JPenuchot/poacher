#pragma once

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <locale>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <asciimath/symbols.hpp>
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
    /// Construct an empty parse result
    constexpr parse_result_t() : _value(std::nullopt) {}

    /// Construct a parse result
    constexpr parse_result_t(T value, std::size_t begin_pos,
                             std::size_t end_pos)
        : _value(std::move(value)), _begin_pos(begin_pos), _end_pos(end_pos) {}

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

  /// Base parse function
  template <typename T> constexpr parse_result_t<T> parse() { return {}; }

  //----------------------------------------------------------------------------
  // identifier = [A-z]

  template <> constexpr parse_result_t<identifier_t> parse<identifier_t>() {
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

  template <> constexpr parse_result_t<symbol_t> parse<symbol_t>() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // number = '-'? [0-9]+ ( '.' [0-9]+ )?

  template <> constexpr parse_result_t<number_t> parse<number_t>() {
    trim_whitespaces();
    std::string_view sv = get_parse_view();

    std::size_t begin = get_pos();

    int sign = 1;
    int val = 0;

    // Should not be empty
    if (sv.empty()) {
      return {};
    }

    // Reading sign
    if (sv.front() == '-') {
      sign = -1;
      sv.remove_prefix(1);
      consume(1);
    }

    // First digit is mandatory
    if (sv.empty() || !std::isdigit(sv.front(), std::locale::classic())) {
      return {};
    }

    // Accumulating into an int...
    while (!sv.empty() && std::isdigit(sv.front(), std::locale::classic())) {
      val *= 10;
      val += sv.front() - '0';

      sv.remove_prefix(1);
      consume(1);
    }

    return {{val * sign}, begin, get_pos()};
  }

  //----------------------------------------------------------------------------
  // constant = number | symbol | identifier

  template <> constexpr parse_result_t<constant_t> parse<constant_t>() {
    // Number parsing attempt
    if (parse_result_t<number_t> number = parse<number_t>(); number) {
      return {
          {*number.get_opt()}, number.get_begin_pos(), number.get_end_pos()};
    }

    // Symbol parsing attempt
    if (parse_result_t<symbol_t> symbol = parse<symbol_t>(); symbol) {
      return {
          {*symbol.get_opt()}, symbol.get_begin_pos(), symbol.get_end_pos()};
    }

    // Identifier parsing attempt
    if (parse_result_t<identifier_t> identifier = parse<identifier_t>();
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

  template <> constexpr parse_result_t<text_t> parse<text_t>() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // binary_op = 'frac' | 'root' | 'stackrel'

  template <> constexpr parse_result_t<binary_op_t> parse<binary_op_t>() {
    trim_whitespaces();

    // TODO

    return {};
  }

  //----------------------------------------------------------------------------
  // binary_expr = binary_op simp simp

  template <> constexpr parse_result_t<binary_expr_t> parse<binary_expr_t>() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Binop parsing attempt
    parse_result_t<binary_op_t> binop_pr = parse<binary_op_t>();
    if (!binop_pr) {
      return {};
    }

    // Simp parsing attempt
    parse_result_t<simp_t> simp_pr_a = parse<simp_t>();
    if (!simp_pr_a) {
      return {};
    }

    // Simp parsing attempt
    parse_result_t<simp_t> simp_pr_b = parse<simp_t>();
    if (!simp_pr_b) {
      return {};
    }

    return {{*binop_pr.get_opt(),
             cest::make_unique<simp_t>(std::move(*simp_pr_a.get_opt())),
             cest::make_unique<simp_t>(std::move(*simp_pr_b.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // unary_op = 'sqrt' | 'text'

  template <> constexpr parse_result_t<unary_op_t> parse<unary_op_t>() {
    // TODO
    return {};
  }

  //----------------------------------------------------------------------------
  // unary_expr = unary_op simp

  template <> constexpr parse_result_t<unary_expr_t> parse<unary_expr_t>() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Unary op parsing attempt
    parse_result_t<unary_op_t> op_pr = parse<unary_op_t>();
    if (!op_pr) {
      return {};
    }

    // Simp parsing attempt
    parse_result_t<simp_t> simp_pr = parse<simp_t>();
    if (!simp_pr) {
      return {};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // rparen = ')' | ']' | '}' | ':)' | ':}'

  template <> constexpr parse_result_t<rparen_t> parse<rparen_t>() {
    trim_whitespaces();
    std::string_view sv;
    std::size_t begin = get_pos();

    // Cannot be empty
    if (sv.empty()) {
      return {};
    }

    // Right bracket symbol static array
    constexpr auto &symbols = symbols::symbols_by_kind<symbols::rightbracket_v>;

    auto it = std::find_if(symbols.begin(), symbols.end(),
                           [&](symbols::symbol_def_t const &def) -> bool {
                             return sv.starts_with(def.input);
                           });

    // No matching symbol
    if (it == symbols.end()) {
      return {};
    }

    return {
        {std::size_t(it - symbols.begin())}, begin, consume(it->input.size())};
  }

  //----------------------------------------------------------------------------
  // lparen = '(' | '[' | '{' | '(:' | '{:'

  template <> constexpr parse_result_t<lparen_t> parse<lparen_t>() {
    trim_whitespaces();
    std::string_view sv;
    std::size_t begin = get_pos();

    // Cannot be empty
    if (sv.empty()) {
      return {};
    }

    // Right bracket symbol static array
    constexpr auto &symbols = symbols::symbols_by_kind<symbols::leftbracket_v>;

    auto it = std::find_if(symbols.begin(), symbols.end(),
                           [&](symbols::symbol_def_t const &def) -> bool {
                             return sv.starts_with(def.input);
                           });

    // No matching symbol
    if (it == symbols.end()) {
      return {};
    }

    consume(it->input.size());
    return {
        {std::size_t(it - symbols.begin())}, begin, consume(it->input.size())};
  }

  //----------------------------------------------------------------------------
  // paren_expr = lparen expr rparen

  template <> constexpr parse_result_t<paren_expr_t> parse<paren_expr_t>() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    parse_result_t<lparen_t> lparen_pr = parse<lparen_t>();
    if (!lparen_pr) {
      return {};
    }

    parse_result_t<expr_t> expr_pr = parse<expr_t>();
    if (!expr_pr) {
      return {};
    }

    parse_result_t<rparen_t> rparen_pr = parse<rparen_t>();
    if (!rparen_pr) {
      return {};
    }

    return {{*lparen_pr.get_opt(),
             cest::make_unique<expr_t>(std::move(*expr_pr.get_opt())),
             *rparen_pr.get_opt()},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // sub =  '_' simp super?

  template <> constexpr parse_result_t<sub_t> parse<sub_t>() {
    trim_whitespaces();
    std::string_view sv = get_parse_view();

    std::size_t begin = get_pos();

    if (sv.empty() || sv.front() != '_') {
      return {};
    }

    sv.remove_prefix(1);
    consume(1);

    parse_result_t<simp_t> simp_pr = parse<simp_t>();
    if (!simp_pr) {
      return {};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // super = '^' simp

  template <> constexpr parse_result_t<super_t> parse<super_t>() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Checking for first char being a hat
    if (std::string_view sv = get_parse_view();
        sv.empty() || sv.front() != '^') {
      return {};
    }

    // Consoom hat
    consume(1);

    // Parse simp
    parse_result_t<simp_t> simp_pr = parse<simp_t>();
    if (!simp_pr) {
      return {};
    }

    return {{cest::make_unique<simp_t>(std::move(*simp_pr.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // fraction = '/' simp

  template <> constexpr parse_result_t<fraction_t> parse<fraction_t>() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Checking for first char being a slash
    if (std::string_view sv = get_parse_view();
        sv.empty() || sv.front() != '/') {
      return {};
    }

    // Consoom slash
    consume(1);

    // Parse simp
    parse_result_t<simp_t> simp_pr = parse<simp_t>();
    if (!simp_pr) {
      return {};
    }

    return {{cest::make_unique<simp_t>(std::move(*simp_pr.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // simp = constant | paren_expr | unary_expr | binary_expr | text

  template <> constexpr parse_result_t<simp_t> parse<simp_t>() {
    trim_whitespaces();
    std::size_t begin = get_pos();

    // Constant parsing attempt
    if (parse_result_t<constant_t> pr = parse<constant_t>(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Paren expr parsing attempt
    if (parse_result_t<paren_expr_t> pr = parse<paren_expr_t>(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Unary expr parsing attempt
    if (parse_result_t<unary_expr_t> pr = parse<unary_expr_t>(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Binary expr parsing attempt
    if (parse_result_t<binary_expr_t> pr = parse<binary_expr_t>(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Text parsing attempt
    if (parse_result_t<text_t> pr = parse<text_t>(); pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // expr = ( simp ( fraction | sub | super ) )+

  template <> constexpr parse_result_t<expr_t> parse<expr_t>() {
    // TODO
    return {};
  }
};

} // namespace asciimath
