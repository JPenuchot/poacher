#include <iterator>
#include <optional>
#include <ranges>
#include <string_view>
#include <vector>

#include <cest/memory.hpp>

// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

// Forward declarations to keep C++ happy :)

struct variable_t;
struct function_t;
struct operator_t;
struct lparen_t;
struct rparen_t;
struct constant_t;
struct failure_t;

/// Token kind to match with token types
enum token_kind_t {
  variable_v,
  function_v,
  operator_v,
  lparen_v,
  rparen_v,
  constant_v,
  failure_v,
};

/// Common type for token specifications. All tokens have an identifier and a
/// kind. More specific data can be attached for children, such as precedence
/// for operators or arity for functions.
struct token_t {
  token_kind_t kind;
  std::string_view text;
  constexpr token_t(token_kind_t kind_, std::string_view text_)
      : kind(kind_), text(text_) {}

  constexpr std::string_view get_text() const { return text; }
  constexpr token_kind_t get_kind() const { return kind; }

  /// Calls visitor with current token casted to its underlaying type.
  template <typename VisitorType> constexpr auto visit(VisitorType visitor) {
    switch (kind) {
    case variable_v:
      return visitor(*(variable_t *)(this));
    case function_v:
      return visitor(*(function_t *)(this));
    case operator_v:
      return visitor(*(operator_t *)(this));
    case lparen_v:
      return visitor(*(lparen_t *)(this));
    case rparen_v:
      return visitor(*(rparen_t *)(this));
    case constant_v:
      return visitor(*(constant_t *)(this));
    case failure_v:
      return visitor(*(failure_t *)(this));
    }
  }

  /// Calls visitor with current token casted to its underlaying const type.
  template <typename VisitorType>
  constexpr auto visit(VisitorType visitor) const {
    switch (kind) {
    case variable_v:
      return visitor(*(variable_t const *)(this));
    case function_v:
      return visitor(*(function_t const *)(this));
    case operator_v:
      return visitor(*(operator_t const *)(this));
    case lparen_v:
      return visitor(*(lparen_t const *)(this));
    case rparen_v:
      return visitor(*(rparen_t const *)(this));
    case constant_v:
      return visitor(*(constant_t const *)(this));
    case failure_v:
      return visitor(*(failure_t const *)(this));
    }
  }
};

/// Variable spec type
struct variable_t : token_t {
  constexpr variable_t(std::string_view identifier)
      : token_t(token_kind_t::variable_v, identifier) {}
};

/// Function spec type
struct function_t : token_t {
  constexpr function_t(std::string_view identifier)
      : token_t(token_kind_t::function_v, identifier) {}
};

/// Operator spec type
struct operator_t : token_t {
  constexpr operator_t(std::string_view identifier)
      : token_t(token_kind_t::operator_v, identifier) {}
};

/// Left parenthesis spec type
struct lparen_t : token_t {
  constexpr lparen_t(std::string_view identifier)
      : token_t(token_kind_t::lparen_v, identifier) {}
};

/// Right parenthesis spec type
struct rparen_t : token_t {
  constexpr rparen_t(std::string_view identifier)
      : token_t(token_kind_t::rparen_v, identifier) {}
};

/// Constant (unsigned integer)
struct constant_t : token_t {
  unsigned value;
  constexpr constant_t(unsigned value_)
      : token_t(constant_v, std::to_string(value_)), value(value_) {}
};

/// Empty token for parsing failure management
struct failure_t : token_t {
  constexpr failure_t() : token_t(failure_v, {}) {}
};

/// Grammar specification that defines a formula to recognizable with the
/// shunting-yard algorithm.
struct grammar_spec_t {
  std::vector<variable_t> variables;
  std::vector<function_t> functions;
  std::vector<operator_t> operators;
  std::vector<lparen_t> lparens;
  std::vector<rparen_t> rparens;
};

/// Type wrapper for parse result. Contains the result itself, and the formula
/// after the parsing.
template <typename ResultType> struct parse_result_t {
  std::optional<ResultType> result;
  std::string_view new_formula;

  constexpr operator bool() const { return result; }
};

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
        return formula.starts_with(token.get_text());
      });

  // If found, remove it from the beginning
  if (token_iterator != std::ranges::end(token_list)) {
    formula.remove_prefix(token_iterator->get_text().size());
  }

  return token_iterator;
}

/// Parses a number. If number parsing fails, a
constexpr cest::unique_ptr<token_t> parse_number(std::string_view &formula) {
  std::size_t number_end = formula.find_last_not_of("0123456789");

  if (number_end == 0) {
    // No number? Failure.
    return cest::make_unique<failure_t>();
  }

  // Accumulate digits
  unsigned result = 0;
  for (std::size_t digit_rank = 0; digit_rank < number_end; digit_rank++) {
    result = (result * 10) + formula[digit_rank];
  }

  // Update formula
  formula.remove_prefix(number_end);
  return cest::make_unique<constant_t>(result);
}

/// Trims whitespaces from the formula.
constexpr void trim_whitespaces(std::string_view &formula) {
  formula.remove_prefix(formula.find_last_not_of(" \t\n"));
}

/// Parses a formula. The result is a vector of pointers to token_spec_t
/// elements contained in the various vectors of spec.
std::vector<token_t const *> constexpr parse_formula(
    std::string_view formula, grammar_spec_t const &spec) {
  // The functions referred to in this algorithm are simple single argument
  // functions such as sine, inverse or factorial.

  // This implementation does not implement composite functions, functions
  // with a variable number of arguments, or unary operators.

  // There are tokens to be read
  while (trim_whitespaces(formula), !formula.empty()) {
    // read a token

    if (false /* token is a number */) {
      // put it into the output queue
    }

    // Token is a function
    if (auto function_spec_iterator =
            parse_token_from_spec_list(formula, spec.functions);
        function_spec_iterator != spec.functions.end()) {
      // Push it onto the operator stack
    }

    // Token is an operator o1
    if (auto operator_spec_iterator =
            parse_token_from_spec_list(formula, spec.operators);
        operator_spec_iterator != spec.operators.end()) {
      operator_t const &o1 = *operator_spec_iterator;

      while (
          false
          // there is an operator o2 at the top of the operator stack which is
          // not a left parenthesis,
          // and (o2 has greater precedence than o1 or
          // (o1 and o2 have the same precedence and o1 is left-associative))
      ) {
        // pop o2 from the operator stack into the output queue
      }
      // push o1 onto the operator stack
    }

    // Token is a left parenthesis (i.e. "(")
    if (auto lparen_token_iterator =
            parse_token_from_spec_list(formula, spec.lparens);
        lparen_token_iterator != spec.lparens.end()) {
      // push it onto the operator stack
    }

    // Token is a right parenthesis (i.e. ")")
    if (auto rparen_token_iterator =
            parse_token_from_spec_list(formula, spec.rparens);
        rparen_token_iterator != spec.rparens.end()) {
      while (false
             // the operator at the top of the operator stack is not a left
             // parenthesis

      ) {
        // {assert the operator stack is not empty}
        /*  If the stack runs out without finding a left parenthesis,
            then there are mismatched parentheses. */
        // pop the operator from the operator stack into the output queue
      }
      // {assert there is a left parenthesis at the top of the operator stack}

      // pop the left parenthesis from the operator stack and discard it

      if(false/* there is a function token at the top of the operator stack */)
      {
        // pop the function from the operator stack into the output queue
      }
    }
  }
  /* After the while loop, pop the remaining items from the operator stack into
   * the output queue. */
  while (false /* there are tokens on the operator stack */) {
    /* If the operator token on the top of the stack is a parenthesis, then
     * there are mismatched parentheses. */

    // {assert the operator on top of the stack is not a (left) parenthesis}

    // pop the operator from the operator stack onto the output queue
  }

  return {};
}

int main() {}
