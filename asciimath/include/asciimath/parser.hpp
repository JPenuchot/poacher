#pragma once

#include <algorithm>
#include <locale>
#include <optional>
#include <string>
#include <string_view>

#include <asciimath/symbols.hpp>
#include <asciimath/types.hpp>
#include <asciimath/utils.hpp>
#include <type_traits>

namespace asciimath {

// AsciiMath grammar:

// identifier = [A-z]
// symbol = /* any string in the symbol table */
// number = '-'? [0-9]+ ( '.' [0-9]+ )?
// constant = number | symbol | identifier
// text = '"' [^"]* ' "'
// binary_op = 'frac' | 'root' | 'stackrel'
// binary_expr = binary_op simp simp
// unary_op = 'sqrt' | 'text'
// unary_expr = unary_op simp
// rparen = ')' | ']' | '}' | ':)' | ':}'
// lparen = '(' | '[' | '{' | '(:' | '{:'
// paren_expr = lparen expr rparen
// sub =  '_' simp super?
// super = '^' simp
// fraction = '/' simp
// simp = constant | paren_expr | unary_expr | binary_expr | text
// expr = ( simp ( fraction | sub | super ) )+

// https://github.com/asciidoctor/asciimath/blob/master/lib/asciimath/parser.rb

//==============================================================================
//
// PARSING STRUCTURES
//

/// Manages parsing state by keeping track of the phrase and its parsing state
class parser_t {
  /// Phrase string view
  std::string_view _phrase;

  /// Current parsing position
  std::size_t _pos;

public:
  constexpr parser_t(std::string_view phrase)
      : _phrase(std::move(phrase)), _pos(0) {}

  /// Get current parsing position
  constexpr std::size_t get_pos() const { return _pos; }

  /// Get a string_view of the AsciiMath phrase
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

  /// Resets parsing position back to a given state
  constexpr void backtrack(std::size_t pos) { _pos = pos; }

  /// Trim whitespaces
  constexpr void trim_whitespaces() {
    _pos = std::find_if_not(_phrase.begin() + _pos, _phrase.end(),
                            &utils::is_blank) -
           _phrase.begin();
  }

  /// Wraps the result of a parsing function
  template <typename T> class parse_result_t {
    std::optional<T> _value;

    /// Begin position of the token in the phrase
    std::size_t _begin_pos;

    /// End position of the token in the phrase
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

    /// Get begin position of the token in the phrase string
    constexpr std::size_t get_begin_pos() const { return _begin_pos; }

    /// Get end position of the token in the phrase string
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

  constexpr void parse_trace(std::string_view msg) const {
    constexpr std::string_view trace_format = "{} [{}]";
    if (!std::is_constant_evaluated()) {
      utils::trace(fmt::format(trace_format, msg, get_parse_view()));
    }
  }

  /// If the character is found, it will consume it and return true. Otherwise
  /// it will return false. Does *not* trim whitespaces.
  constexpr bool parse_char(std::string_view::value_type c) {
    utils::trace();
    if (get_parse_view().starts_with(c)) {
      consume(1);
      return true;
    }
    return false;
  }

  /// If the string is found, it will consume it and return true. Otherwise it
  /// will return false. Does *not* trim whitespaces.
  constexpr bool parse_string(std::string_view const &sv) {
    utils::trace();
    if (get_parse_view().starts_with(sv)) {
      consume(sv.size());
      return true;
    }
    return false;
  }

  /// Tries to parse a symbol of given kind, then consumes it and returns the
  /// index of the symbol definition in symbols_by_kind<Kind>. Otherwise returns
  /// an empty parse result.
  template <symbols::symbol_kind_t Kind>
  constexpr parse_result_t<std::size_t> parse_symbol_by_kind() {
    utils::trace();
    trim_whitespaces();

    std::string_view sv;
    std::size_t const begin = get_pos();

    // Cannot be empty
    if (sv.empty()) {
      return {};
    }

    // Right bracket symbol static array
    constexpr auto &symbols = symbols::symbols_by_kind<Kind>;

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

  /// Parse function template. Specializations for this functions must either
  /// return a value and consume the characters associated to it, or return an
  /// empty parse result and keep the parser state unchanged.
  template <typename T> constexpr parse_result_t<T> parse();

  constexpr parse_result_t<ast::expr_t> parse_expr() {
    return parse<ast::expr_t>();
  }

  //----------------------------------------------------------------------------
  // identifier = [A-z]

  template <>
  constexpr parse_result_t<ast::identifier_t> parse<ast::identifier_t>() {
    parse_trace("identifier = [A-z]");

    std::size_t const begin = get_pos();
    std::string_view sv = get_parse_view();

    // Beginning of the identifier
    std::string_view::const_iterator sv_begin = sv.begin();

    // Find first char that isn't a letter
    std::string_view::const_iterator sv_end = std::find_if_not(
        sv.begin(), sv.end(), [](std::string_view::value_type c) -> bool {
          return utils::is_alpha(c);
        });

    // Empty identifier? Empty result.
    if (sv_end == sv_begin) {
      return {};
    }

    // Non-empty identifier
    return {ast::identifier_t{{sv_begin, sv_end}}, get_pos(),
            consume(sv_end - sv_begin)};
  }

  //----------------------------------------------------------------------------
  // symbol = /* any string in the symbol table */

  template <> constexpr parse_result_t<ast::symbol_t> parse<ast::symbol_t>() {
    parse_trace("symbol = /* any string in the symbol table */");

    // TODO

    // Don't know what to do
    return {};
  }

  //----------------------------------------------------------------------------
  // number = '-'? [0-9]+ ( '.' [0-9]+ )?

  template <> constexpr parse_result_t<ast::number_t> parse<ast::number_t>() {
    parse_trace("number = '-'? [0-9]+ ( '.' [0-9]+ )?");

    std::size_t const begin = get_pos();

    std::string_view sv = get_parse_view();
    std::string_view::const_iterator const parse_begin = sv.begin();

    int val = 0;
    bool minus = false;

    if (sv.starts_with('-')) {
      minus = true;
      sv.remove_prefix(1);
      consume(1);
    }

    // Should not be empty
    if (sv.empty()) {
      backtrack(begin);
      return {};
    }

    // First digit is mandatory
    if (sv.empty() || !utils::is_digit(sv.front())) {
      backtrack(begin);
      return {};
    }

    // Accumulating into an int...
    while (!sv.empty() && utils::is_digit(sv.front())) {
      val *= 10;
      val += sv.front() - '0';

      sv.remove_prefix(1);
      consume(1);
    }

    return {{minus ? -val : val}, begin, std::size_t(sv.begin() - parse_begin)};
  }

  //----------------------------------------------------------------------------
  // constant = number | symbol | identifier

  template <>
  constexpr parse_result_t<ast::constant_t> parse<ast::constant_t>() {
    parse_trace("constant = number | symbol | identifier");

    std::size_t const begin = get_pos();

    // Number parsing attempt
    if (parse_result_t<ast::number_t> number =
            (trim_whitespaces(), parse<ast::number_t>());
        number) {
      return {
          {*number.get_opt()}, number.get_begin_pos(), number.get_end_pos()};
    }

    // Symbol parsing attempt
    if (parse_result_t<ast::symbol_t> symbol =
            (trim_whitespaces(), parse<ast::symbol_t>());
        symbol) {
      return {
          {*symbol.get_opt()}, symbol.get_begin_pos(), symbol.get_end_pos()};
    }

    // Identifier parsing attempt
    if (parse_result_t<ast::identifier_t> identifier =
            (trim_whitespaces(), parse<ast::identifier_t>());
        identifier) {
      return {{*identifier.get_opt()},
              identifier.get_begin_pos(),
              identifier.get_end_pos()};
    }

    // Failure
    backtrack(begin);
    return {};
  }

  //----------------------------------------------------------------------------
  // text = '"' [^"]* '"'

  template <> constexpr parse_result_t<ast::text_t> parse<ast::text_t>() {
    parse_trace("text = '\"' [^\"]* '\"'");

    std::size_t const begin = get_pos();

    // TODO

    if (!(trim_whitespaces(), parse_char('\"'))) {
      backtrack(begin);
      return {};
    }

    backtrack(begin);
    return {};
  }

  //----------------------------------------------------------------------------
  // binary_op = 'frac' | 'root' | 'stackrel'

  template <>
  constexpr parse_result_t<ast::binary_op_t> parse<ast::binary_op_t>() {
    parse_trace("binary_op = 'frac' | 'root' | 'stackrel'");

    std::size_t const begin = get_pos();

    parse_result_t<std::size_t> binop_pr =
        (trim_whitespaces(),
         parse_symbol_by_kind<symbols::symbol_kind_t::binary_v>());

    if (!binop_pr) {
      backtrack(begin);
      return {};
    }

    return {{*binop_pr.get_opt()}, begin, get_pos()};
  }

  //----------------------------------------------------------------------------
  // binary_expr = binary_op simp simp

  template <>
  constexpr parse_result_t<ast::binary_expr_t> parse<ast::binary_expr_t>() {
    parse_trace("binary_expr = binary_op simp simp");

    std::size_t const begin = get_pos();

    // Binop parsing attempt
    parse_result_t<ast::binary_op_t> binop_pr =
        (trim_whitespaces(), parse<ast::binary_op_t>());
    if (!binop_pr) {
      backtrack(begin);
      return {};
    }

    // Simp parsing attempt
    parse_result_t<ast::simp_t> simp_pr_a =
        (trim_whitespaces(), parse<ast::simp_t>());
    if (!simp_pr_a) {
      backtrack(begin);
      return {};
    }

    // Simp parsing attempt
    parse_result_t<ast::simp_t> simp_pr_b =
        (trim_whitespaces(), parse<ast::simp_t>());
    if (!simp_pr_b) {
      backtrack(begin);
      return {};
    }

    return {{*binop_pr.get_opt(),
             cest::make_unique<ast::simp_t>(std::move(*simp_pr_a.get_opt())),
             cest::make_unique<ast::simp_t>(std::move(*simp_pr_b.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // unary_op = 'sqrt' | 'text'

  template <>
  constexpr parse_result_t<ast::unary_op_t> parse<ast::unary_op_t>() {
    parse_trace("unary_op = 'sqrt' | 'text'");

    if (parse_result_t<std::size_t> pr =
            (trim_whitespaces(),
             parse_symbol_by_kind<symbols::symbol_kind_t::unary_v>());
        pr) {
      return {{*pr.get_opt()}, pr.get_begin_pos(), pr.get_end_pos()};
    }

    return {};
  }

  //----------------------------------------------------------------------------
  // unary_expr = unary_op simp

  template <>
  constexpr parse_result_t<ast::unary_expr_t> parse<ast::unary_expr_t>() {
    parse_trace("unary_expr = unary_op simp");

    std::size_t const begin = get_pos();

    // Unary op parsing attempt
    parse_result_t<ast::unary_op_t> op_pr =
        (trim_whitespaces(), parse<ast::unary_op_t>());
    if (!op_pr) {
      backtrack(begin);
      return {};
    }

    // Simp parsing attempt
    parse_result_t<ast::simp_t> simp_pr =
        (trim_whitespaces(), parse<ast::simp_t>());
    if (!simp_pr) {
      backtrack(begin);
      return {};
    }

    return {{op_pr.get_opt().value(),
             cest::make_unique<ast::simp_t>(std::move(*simp_pr.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // rparen = ')' | ']' | '}' | ':)' | ':}'

  template <> constexpr parse_result_t<ast::rparen_t> parse<ast::rparen_t>() {
    parse_trace("rparen = ')' | ']' | '}' | ':)' | ':}'");

    if (parse_result_t<std::size_t> pr =
            (trim_whitespaces(),
             parse_symbol_by_kind<symbols::rightbracket_v>());
        pr) {
      return {{*pr.get_opt()}, pr.get_begin_pos(), pr.get_end_pos()};
    }
    return {};
  }

  //----------------------------------------------------------------------------
  // lparen = '(' | '[' | '{' | '(:' | '{:'

  template <> constexpr parse_result_t<ast::lparen_t> parse<ast::lparen_t>() {
    parse_trace("lparen = '(' | '[' | '{' | '(:' | '{:'");

    if (parse_result_t<std::size_t> pr =
            (trim_whitespaces(),
             parse_symbol_by_kind<symbols::leftbracket_v>());
        pr) {
      return {{*pr.get_opt()}, pr.get_begin_pos(), pr.get_end_pos()};
    }
    return {};
  }

  //----------------------------------------------------------------------------
  // paren_expr = lparen expr rparen

  template <>
  constexpr parse_result_t<ast::paren_expr_t> parse<ast::paren_expr_t>() {
    parse_trace("paren_expr = lparen expr rparen");

    std::size_t const begin = get_pos();

    parse_result_t<ast::lparen_t> lparen_pr =
        (trim_whitespaces(), parse<ast::lparen_t>());
    if (!lparen_pr) {
      backtrack(begin);
      return {};
    }

    parse_result_t<ast::expr_t> expr_pr =
        (trim_whitespaces(), parse<ast::expr_t>());
    if (!expr_pr) {
      backtrack(begin);
      return {};
    }

    parse_result_t<ast::rparen_t> rparen_pr =
        (trim_whitespaces(), parse<ast::rparen_t>());
    if (!rparen_pr) {
      backtrack(begin);
      return {};
    }

    return {{*lparen_pr.get_opt(),
             cest::make_unique<ast::expr_t>(std::move(*expr_pr.get_opt())),
             *rparen_pr.get_opt()},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // sub =  '_' simp super?

  template <> constexpr parse_result_t<ast::sub_t> parse<ast::sub_t>() {
    parse_trace("sub =  '_' simp super?");

    std::size_t const begin = get_pos();

    if (!(trim_whitespaces(), parse_char('_'))) {
      return {};
    }

    parse_result_t<ast::simp_t> simp_pr =
        (trim_whitespaces(), parse<ast::simp_t>());
    if (!simp_pr) {
      backtrack(begin);
      return {};
    }

    parse_result_t<ast::super_t> super_pr =
        (trim_whitespaces(), parse<ast::super_t>());

    if (super_pr) {
      return {{cest::make_unique<ast::simp_t>(std::move(*simp_pr.get_opt())),
               cest::make_unique<ast::super_t>(std::move(*super_pr.get_opt()))},
              begin,
              get_pos()};
    }

    return {{cest::make_unique<ast::simp_t>(std::move(*simp_pr.get_opt())),
             std::nullopt},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // super = '^' simp

  template <> constexpr parse_result_t<ast::super_t> parse<ast::super_t>() {
    parse_trace("super = '^' simp");

    std::size_t const begin = get_pos();

    // Checking for first char being a hat
    if (!(trim_whitespaces(), parse_char('^'))) {
      backtrack(begin);
      return {};
    }

    // Parse simp
    parse_result_t<ast::simp_t> simp_pr =
        (trim_whitespaces(), parse<ast::simp_t>());
    if (!simp_pr) {
      backtrack(begin);
      return {};
    }

    return {{cest::make_unique<ast::simp_t>(std::move(*simp_pr.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // fraction = '/' simp

  template <>
  constexpr parse_result_t<ast::fraction_t> parse<ast::fraction_t>() {
    parse_trace("fraction = '/' simp");

    std::size_t const begin = get_pos();

    // Checking for first char being a slash
    if (!(trim_whitespaces(), parse_char('/'))) {
      backtrack(begin);
      return {};
    }

    // Parse simp
    parse_result_t<ast::simp_t> simp_pr =
        (trim_whitespaces(), parse<ast::simp_t>());
    if (!simp_pr) {
      backtrack(begin);
      return {};
    }

    return {{cest::make_unique<ast::simp_t>(std::move(*simp_pr.get_opt()))},
            begin,
            get_pos()};
  }

  //----------------------------------------------------------------------------
  // simp = constant | paren_expr | unary_expr | binary_expr | text

  template <> constexpr parse_result_t<ast::simp_t> parse<ast::simp_t>() {
    parse_trace(
        "simp = constant | paren_expr | unary_expr | binary_expr | text");

    std::size_t const begin = get_pos();

    // Constant parsing attempt
    if (parse_result_t<ast::constant_t> pr =
            (trim_whitespaces(), parse<ast::constant_t>());
        pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Paren expr parsing attempt
    if (parse_result_t<ast::paren_expr_t> pr =
            (trim_whitespaces(), parse<ast::paren_expr_t>());
        pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Unary expr parsing attempt
    if (parse_result_t<ast::unary_expr_t> pr =
            (trim_whitespaces(), parse<ast::unary_expr_t>());
        pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Binary expr parsing attempt
    if (parse_result_t<ast::binary_expr_t> pr =
            (trim_whitespaces(), parse<ast::binary_expr_t>());
        pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    // Text parsing attempt
    if (parse_result_t<ast::text_t> pr =
            (trim_whitespaces(), parse<ast::text_t>());
        pr) {
      return {{std::move(*pr.get_opt())}, begin, get_pos()};
    }

    backtrack(begin);
    return {};
  }

  //----------------------------------------------------------------------------
  // expr = ( simp ( fraction | sub | super ) )+

  template <> constexpr parse_result_t<ast::expr_t> parse<ast::expr_t>() {
    parse_trace("expr = ( simp ( fraction | sub | super ) )+");

    std::size_t const begin = get_pos();

    ast::expr_t result;

    trim_whitespaces();
    for (parse_result_t<ast::simp_t> simp_pr = parse<ast::simp_t>(); simp_pr;
         simp_pr = (trim_whitespaces(), parse<ast::simp_t>())) {

      ast::expr_element_t element;

      element.simp_ptr =
          cest::make_unique<ast::simp_t>(std::move(*simp_pr.get_opt()));

      // Attempting to parse a fraction
      if (parse_result_t<ast::fraction_t> pr =
              (trim_whitespaces(), parse<ast::fraction_t>());
          pr) {
        element.next = std::move(*pr.get_opt());
        result.content.push_back(std::move(element));
      }

      // Attempting to parse a sub
      else if (parse_result_t<ast::sub_t> pr =
                   (trim_whitespaces(), parse<ast::sub_t>());
               pr) {
        element.next = std::move(*pr.get_opt());
        result.content.push_back(std::move(element));
      }

      // Attempting to parse a super
      else if (parse_result_t<ast::super_t> pr =
                   (trim_whitespaces(), parse<ast::super_t>());
               pr) {
        element.next = std::move(*pr.get_opt());
        result.content.push_back(std::move(element));
      }
    }

    // Failure if empty.
    if (result.content.empty()) {
      backtrack(begin);
      return {};
    }

    return {std::move(result), begin, get_pos()};
  }
};

} // namespace asciimath
