#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <vector>

#include <fmt/core.h>

#include <cest/memory.hpp>

// Implementation based on:
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

// Forward declarations to keep C++ happy :)

struct failure_t;
struct variable_t;
struct function_t;
struct operator_t;
struct lparen_t;
struct rparen_t;
struct constant_t;

/// Token kind to match with token types
enum token_kind_t {
  failure_v,
  variable_v,
  function_v,
  operator_v,
  lparen_v,
  rparen_v,
  constant_v,
};

/// Common type for token specifications. All tokens have an identifier and a
/// kind. More specific data can be attached for children, such as precedence
/// for operators or arity for functions.
struct token_t {
  token_kind_t kind;
  std::string_view text;

  constexpr virtual ~token_t() = default;

  constexpr token_t(token_kind_t kind_, std::string_view text_)
      : kind(kind_), text(text_) {}

  /// Calls visitor with current token casted to its underlaying type.
  template <typename VisitorType> constexpr auto visit(VisitorType visitor);

  /// Calls visitor with current token casted to its underlaying const type.
  template <typename VisitorType>
  constexpr auto visit(VisitorType visitor) const;

  /// Returns true if a valid result is held by the object.
  constexpr operator bool() const { return kind != failure_v; }
};

/// Empty token for parsing failure management
struct failure_t : token_t {
  constexpr failure_t() : token_t(failure_v, {}) {}
};

/// Variable spec type
struct variable_t : token_t {
  constexpr variable_t(std::string_view identifier)
      : token_t(variable_v, identifier) {}
};

/// Function spec type
struct function_t : token_t {
  constexpr function_t(std::string_view identifier)
      : token_t(function_v, identifier) {}
};

/// Operator associativity
enum operator_associativity_t { left_v, right_v };

/// Operator spec type
struct operator_t : token_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr operator_t(std::string_view identifier,
                       operator_associativity_t associativity_,
                       unsigned precedence_)
      : token_t(operator_v, identifier), associativity(associativity_),
        precedence(precedence_) {}
};

/// Left parenthesis spec type
struct lparen_t : token_t {
  constexpr lparen_t(std::string_view identifier)
      : token_t(lparen_v, identifier) {}
};

/// Right parenthesis spec type
struct rparen_t : token_t {
  constexpr rparen_t(std::string_view identifier)
      : token_t(rparen_v, identifier) {}
};

/// Constant (unsigned integer)
struct constant_t : token_t {
  unsigned value;
  constexpr constant_t(unsigned value_)
      : token_t(constant_v, ""), value(value_) {}
};

/// Calls visitor with current token casted to its underlaying type.
template <typename VisitorType>
constexpr auto token_t::visit(VisitorType visitor) {
  switch (kind) {
  case failure_v:
    return visitor(static_cast<failure_t &>(*this));
  case variable_v:
    return visitor(static_cast<variable_t &>(*this));
  case function_v:
    return visitor(static_cast<function_t &>(*this));
  case operator_v:
    return visitor(static_cast<operator_t &>(*this));
  case lparen_v:
    return visitor(static_cast<lparen_t &>(*this));
  case rparen_v:
    return visitor(static_cast<rparen_t &>(*this));
  case constant_v:
    return visitor(static_cast<constant_t &>(*this));
  }
}

/// Calls visitor with current token casted to its underlaying const type.
template <typename VisitorType>
constexpr auto token_t::visit(VisitorType visitor) const {
  switch (kind) {
  case failure_v:
    return visitor(static_cast<failure_t const &>(*this));
  case variable_v:
    return visitor(static_cast<variable_t const &>(*this));
  case function_v:
    return visitor(static_cast<function_t const &>(*this));
  case operator_v:
    return visitor(static_cast<operator_t const &>(*this));
  case lparen_v:
    return visitor(static_cast<lparen_t const &>(*this));
  case rparen_v:
    return visitor(static_cast<rparen_t const &>(*this));
  case constant_v:
    return visitor(static_cast<constant_t const &>(*this));
  }
}

/// Tries to parse a token from the given token list and returns the iterator to
/// it. If found, it will be removed from the beginning of formula. If not,
/// formula remains unchanged and the iterator will be the end of the range.
/// Whitespaces are not trimmed by the function either before or after the
/// parsing.
template <std::ranges::range RangeType>
constexpr std::ranges::iterator_t<RangeType const>
parse_token_from_spec_list(std::string_view &formula,
                           RangeType const &token_list) {
  // Try to find the token from the list
  std::ranges::iterator_t<RangeType const> token_iterator =
      std::ranges::find_if(token_list, [&](token_t const &token) {
        return formula.starts_with(token.text);
      });

  // If found, remove it from the beginning
  if (token_iterator != std::ranges::end(token_list)) {
    formula.remove_prefix(token_iterator->text.size());
  }

  return token_iterator;
}

constexpr bool is_digit(char c) { return '0' <= c && c <= '9'; }

/// Parses a number.
/// If number parsing fails, the result will be a failure_t object.
constexpr cest::unique_ptr<token_t> parse_number(std::string_view &text) {
  // Checking for presence of a digit
  if (text.empty() || !is_digit(text.front())) {
    return cest::make_unique<failure_t>();
  }

  // Accumulate digits
  unsigned result = 0;
  while (!text.empty() && is_digit(text.front())) {
    result *= 10;
    result += text.front() - '0';

    // Update text

    text.remove_prefix(1);
  }

  return cest::make_unique<constant_t>(result);
}

/// Trims whitespaces from the formula.
constexpr void trim_whitespaces(std::string_view &formula) {
  if (std::size_t n = formula.find_last_not_of(" \t\n");
      n != std::string_view::npos) {
    formula.remove_prefix(n);
  }
}

/// Grammar specification that defines a formula to recognizable with the
/// shunting-yard algorithm.
struct grammar_spec_t {
  std::vector<variable_t> variables;
  std::vector<function_t> functions;
  std::vector<operator_t> operators;
  std::vector<lparen_t> lparens;
  std::vector<rparen_t> rparens;
};

/// Represents the parsing result of parse_formula. Constants holds the
/// constant objects that may be generated by parse_formula.
struct shunting_yard_result_t {
  std::vector<token_t const *> output_queue;
  std::vector<constant_t> constants;
};

/// Parses a formula. The result is a vector of pointers to token_spec_t
/// elements contained in the various vectors of spec.
shunting_yard_result_t constexpr parse_formula(std::string_view formula,
                                               grammar_spec_t const &spec) {
  // The functions referred to in this algorithm are simple single argument
  // functions such as sine, inverse or factorial.

  // This implementation does not implement composite functions, functions
  // with a variable number of arguments, or unary operators.

  shunting_yard_result_t result;
  std::vector<token_t const *> operator_stack;

  // There are tokens to be read
  while (trim_whitespaces(formula), !formula.empty()) {
    std::size_t formula_size = formula.size();

    if (!std::is_constant_evaluated()) {
      fmt::print(stderr, "Formula: \"{}\"\n", formula);
    }
    // read a token

    // Token is a number constant
    if (parse_number(formula)->visit(
            [&]<typename TokenType>(TokenType const &parsed_item) {
              if constexpr (std::is_same_v<constant_t, TokenType>) {
                // The constant must be stored first
                result.constants.push_back(parsed_item);
                return true;
              }
              return false;
            })) {
      // Put it into the output queue
      result.output_queue.push_back(std::addressof(result.constants.back()));
    }

    // Token is a function
    else if (auto function_spec_iterator =
                 parse_token_from_spec_list(formula, spec.functions);
             function_spec_iterator != spec.functions.end()) {
      // Push it onto the operator stack
      operator_stack.push_back(function_spec_iterator.base());
    }

    // Token is an operator o1
    else if (auto o1_spec_iterator =
                 parse_token_from_spec_list(formula, spec.operators);
             o1_spec_iterator != spec.operators.end()) {
      operator_t const &o1 = *o1_spec_iterator;

      // there is an operator o2 at the top of the operator stack
      // which is not a left parenthesis,
      while (!operator_stack.empty() &&
             operator_stack.back()->kind != lparen_v) {
        if (operator_stack.back()->visit(
                [&]<typename O2Type>(O2Type const &o2_as_auto) -> bool {
                  if constexpr (std::is_same_v<O2Type, operator_t>) {
                    operator_t const &o2_as_operator = o2_as_auto;
                    // and (o2 has greater precedence than o1 or
                    // (o1 and o2 have the same precedence and o1 is
                    // left-associative))
                    if (o2_as_operator.precedence > o1.precedence ||
                        (o1.precedence == o2_as_operator.precedence &&
                         o1.associativity == left_v)) {
                      return true;
                    }
                  } else if constexpr (std::is_same_v<O2Type, function_t>) {
                    // NOTE: We assume functions have greater precedence
                    return true;
                  }
                  return false;
                })) {
          // pop o2 from the operator stack into the output queue
          result.output_queue.push_back(operator_stack.back());
          operator_stack.pop_back();
        }
      }
      // push o1 onto the operator stack
      operator_stack.push_back(o1_spec_iterator.base());
    }

    // Token is a left parenthesis (i.e. "(")
    else if (auto lparen_token_iterator =
                 parse_token_from_spec_list(formula, spec.lparens);
             lparen_token_iterator != spec.lparens.end()) {
      // push it onto the operator stack
      operator_stack.push_back(lparen_token_iterator.base());
    }

    // Token is a right parenthesis (i.e. ")")
    else if (auto rparen_token_iterator =
                 parse_token_from_spec_list(formula, spec.rparens);
             rparen_token_iterator != spec.rparens.end()) {
      // the operator at the top of the operator stack is not a left parenthesis
      while (operator_stack.empty() ||
             operator_stack.back()->kind != lparen_v) {

        // {assert the operator stack is not empty}
        if (operator_stack.empty()) {
          // If the stack runs out without finding a left parenthesis, then
          // there are mismatched parentheses.
          throw;
        }
        // pop the operator from the operator stack into the output queue
        result.output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
      }

      // {assert there is a left parenthesis at the top of the operator stack}
      if (operator_stack.empty() || operator_stack.back()->kind != lparen_v) {
        throw;
      }

      // pop the left parenthesis from the operator stack and discard it
      operator_stack.pop_back();

      // there is a function token at the top of the operator stack
      if (!operator_stack.empty() &&
          operator_stack.back()->kind == function_v) {
        // pop the function from the operator stack into the output queue
        result.output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
      }
    }
  }
  /* After the while loop, pop the remaining items from the operator stack into
   * the output queue. */

  // there are tokens on the operator stack
  while (!operator_stack.empty()) {
    // If the operator token on the top of the stack is a parenthesis, then
    // there are mismatched parentheses.

    // {assert the operator on top of the stack is not a (left) parenthesis}
    if (operator_stack.back()->kind == lparen_v) {
      throw;
    }

    // pop the operator from the operator stack onto the output queue
    result.output_queue.push_back(operator_stack.back());
    operator_stack.pop_back();
  }

  return result;
}

constexpr bool foo() {
  grammar_spec_t rubbish_algebra{.variables = {},
                                 .functions = {},
                                 .operators =
                                     {
                                         operator_t("+", left_v, 10),
                                         operator_t("-", left_v, 10),
                                         operator_t("*", left_v, 20),
                                         operator_t("/", left_v, 20),
                                         operator_t("^", right_v, 30),
                                     },
                                 .lparens = {lparen_t("(")},
                                 .rparens = {rparen_t(")")}};

  shunting_yard_result_t val = parse_formula("1 + 1 / 2", rubbish_algebra);

  return true;
}

int main() { constexpr bool val = foo(); }
