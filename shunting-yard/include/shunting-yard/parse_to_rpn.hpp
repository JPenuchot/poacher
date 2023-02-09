#pragma once

// Implementation based on:
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

#include <iterator>
#include <ranges>
#include <string_view>
#include <variant>
#include <vector>

#include <cest/memory.hpp>

#include <kumi/tuple.hpp>

#include <fmt/core.h>

namespace shunting_yard {

/// unique_ptr implementation namespace selector
namespace unique_ptr_impl = cest;

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
struct token_base_t {
  token_kind_t kind;
  std::string_view text;

  constexpr virtual ~token_base_t() = default;

  constexpr token_base_t(token_kind_t kind_, std::string_view text_)
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
struct failure_t : token_base_t {
  constexpr failure_t() : token_base_t(failure_v, {}) {}
};

/// Variable spec type
struct variable_t : token_base_t {
  constexpr variable_t(std::string_view identifier)
      : token_base_t(variable_v, identifier) {}
};

/// Function spec type
struct function_t : token_base_t {
  constexpr function_t(std::string_view identifier)
      : token_base_t(function_v, identifier) {}
};

/// Operator associativity
enum operator_associativity_t { left_v, right_v };

/// Operator spec type
struct operator_t : token_base_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr operator_t(std::string_view identifier,
                       operator_associativity_t associativity_,
                       unsigned precedence_)
      : token_base_t(operator_v, identifier), associativity(associativity_),
        precedence(precedence_) {}
};

/// Left parenthesis spec type
struct lparen_t : token_base_t {
  constexpr lparen_t(std::string_view identifier)
      : token_base_t(lparen_v, identifier) {}
};

/// Right parenthesis spec type
struct rparen_t : token_base_t {
  constexpr rparen_t(std::string_view identifier)
      : token_base_t(rparen_v, identifier) {}
};

/// Constant (unsigned integer)
struct constant_t : token_base_t {
  unsigned value;
  constexpr constant_t(unsigned value_, std::string_view number)
      : token_base_t(constant_v, number), value(value_) {}
};

template <typename VisitorType>
constexpr auto token_base_t::visit(VisitorType visitor) {
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

template <typename VisitorType>
constexpr auto token_base_t::visit(VisitorType visitor) const {
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

/// Grammar specification that defines a formula to recognizable with the
/// shunting-yard algorithm.
struct token_specification_t {
  std::vector<variable_t> variables;
  std::vector<function_t> functions;
  std::vector<operator_t> operators;
  std::vector<lparen_t> lparens;
  std::vector<rparen_t> rparens;
};

/// Represents the parsing result of parse_formula.
struct rpn_result_t {
  /// List of tokens (RPN notation)
  std::vector<token_base_t const *> output_queue;

  /// Polymorphic buffer for tokens that aren't already held in the
  /// grammar_spec_t object (constants)
  std::vector<unique_ptr_impl::unique_ptr<token_base_t>> token_memory;
};

/// Literal version for token base type (see token_t)
struct literal_token_base_t {
  std::string_view text;
};

template <typename T>
concept is_literal_token_c = requires(T e) {
                               {
                                 e.text
                                 } -> std::convertible_to<std::string_view>;
                             };

/// Literal version for empty token for parsing failure management
struct literal_failure_t : literal_token_base_t {
  constexpr literal_failure_t() : literal_token_base_t{{}} {}

  /// Conversion function
  constexpr literal_failure_t(failure_t const &) : literal_failure_t() {}
};

/// Literal version for variable spec type
struct literal_variable_t : literal_token_base_t {
  constexpr literal_variable_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_variable_t(variable_t const &non_literal)
      : literal_variable_t(non_literal.text) {}
};

/// Literal version for function spec type
struct literal_function_t : literal_token_base_t {
  constexpr literal_function_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_function_t(function_t const &non_literal)
      : literal_function_t(non_literal.text) {}
};

/// Literal version for operator spec type
struct literal_operator_t : literal_token_base_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr literal_operator_t(std::string_view identifier,
                               operator_associativity_t associativity_,
                               unsigned precedence_)
      : literal_token_base_t{identifier}, associativity(associativity_),
        precedence(precedence_) {}

  /// Conversion function
  constexpr literal_operator_t(operator_t const &non_literal)
      : literal_operator_t(non_literal.text, non_literal.associativity,
                           non_literal.precedence) {}
};

/// Literal version for left parenthesis spec type
struct literal_lparen_t : literal_token_base_t {
  constexpr literal_lparen_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_lparen_t(lparen_t const &non_literal)
      : literal_lparen_t(non_literal.text) {}
};

/// Literal version for right parenthesis spec type
struct literal_rparen_t : literal_token_base_t {
  constexpr literal_rparen_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_rparen_t(rparen_t const &non_literal)
      : literal_rparen_t(non_literal.text) {}
};

/// Constant (unsigned integer)
struct literal_constant_t : literal_token_base_t {
  unsigned value;
  constexpr literal_constant_t(unsigned value_, std::string_view number)
      : literal_token_base_t{number}, value(value_) {}

  /// Conversion function
  constexpr literal_constant_t(constant_t const &non_literal)
      : literal_constant_t(non_literal.value, non_literal.text) {}
};

/// Literal generic type for a token.
using literal_token_t =
    std::variant<literal_failure_t, literal_variable_t, literal_function_t,
                 literal_operator_t, literal_lparen_t, literal_rparen_t,
                 literal_constant_t>;

// Sanity check
namespace _test {
constexpr literal_token_t test_literal_token(literal_constant_t(1, "one"));
}

/// Helper kind getter for literal_failure_t.
constexpr token_kind_t get_kind(literal_failure_t const &) { return failure_v; }
/// Helper kind getter for literal_variable_t.
constexpr token_kind_t get_kind(literal_variable_t const &) {
  return variable_v;
}
/// Helper kind getter for literal_function_t.
constexpr token_kind_t get_kind(literal_function_t const &) {
  return function_v;
}
/// Helper kind getter for literal_operator_t.
constexpr token_kind_t get_kind(literal_operator_t const &) {
  return operator_v;
}
/// Helper kind getter for literal_lparen_t.
constexpr token_kind_t get_kind(literal_lparen_t const &) { return lparen_v; }
/// Helper kind getter for literal_rparen_t.
constexpr token_kind_t get_kind(literal_rparen_t const &) { return rparen_v; }
/// Helper kind getter for literal_constant_t.
constexpr token_kind_t get_kind(literal_constant_t const &) {
  return constant_v;
}

/// Tries to parse a token from the token list and returns an iterator to it.
/// - If found, it will be removed from the beginning of formula.
/// - If not, formula remains unchanged and the iterator will be the end of the
///   range.
/// Whitespaces are not trimmed by the function either before or after the
/// parsing.
template <std::ranges::range RangeType>
constexpr std::ranges::iterator_t<RangeType const>
parse_token_from_spec_list(std::string_view &formula,
                           RangeType const &token_list) {
  // Try to find the token from the list
  std::ranges::iterator_t<RangeType const> token_iterator =
      std::ranges::find_if(token_list, [&](token_base_t const &token) {
        return formula.starts_with(token.text);
      });

  // If found, remove it from the beginning
  if (token_iterator != std::ranges::end(token_list)) {
    formula.remove_prefix(token_iterator->text.size());
  }

  return token_iterator;
}

/// Tries to parse a number.
/// - If found, it will return a constant_t object holding its value, and remove
///   the number from the beginning of the formula.
/// - If not, formula remains unchanged and it will return a failure_t object.
/// Whitespaces are not trimmed by the function either before or after the
/// parsing.
constexpr unique_ptr_impl::unique_ptr<token_base_t>
parse_number(std::string_view &text) {
  // Checking for presence of a digit
  std::size_t find_result = text.find_first_not_of("0123456789");

  if (find_result == 0) {
    return unique_ptr_impl::make_unique<failure_t>();
  }

  // No character other than a digit means it's all digit
  std::size_t number_end_pos =
      find_result == std::string_view::npos ? text.size() : find_result;

  // Accumulate digits
  unsigned result = 0;
  for (std::size_t digit_index = 0; digit_index < number_end_pos;
       digit_index++) {
    result += result * 10 + (text[digit_index] - '0');
  }

  std::string_view number_view = text.substr(0, number_end_pos);
  text.remove_prefix(number_end_pos);
  return unique_ptr_impl::make_unique<constant_t>(result, number_view);
}

/// Trims characters from ignore_list at the beginning of the formula.
constexpr void trim_formula(std::string_view &formula,
                            std::string_view ignore_list = ", \t\n") {
  if (std::size_t n = formula.find_first_not_of(ignore_list);
      n != std::string_view::npos) {
    formula.remove_prefix(n);
  }
}

/// Parses a formula. The result is a vector of pointers to token_spec_t
/// elements contained in the various vectors of spec.
rpn_result_t constexpr parse_to_rpn(std::string_view formula,
                                    token_specification_t const &spec) {
  // The functions referred to in this algorithm are simple single argument
  // functions such as sine, inverse or factorial.

  // This implementation does not implement composite functions, functions
  // with a variable number of arguments, or unary operators.

  rpn_result_t result;
  std::vector<token_base_t const *> operator_stack;

  if (!std::is_constant_evaluated()) {
    fmt::print("Starting formula: \"{}\"\n", formula);
  }

  // There are tokens to be read
  while (trim_formula(formula), !formula.empty()) {
    if (!std::is_constant_evaluated()) {
      fmt::print("- Current step: \"{}\"\n", formula);
    }
    // read a token

    // Token is a number constant
    if (unique_ptr_impl::unique_ptr<token_base_t> parsed_token =
            parse_number(formula);
        parsed_token->kind == constant_v) {
      // Put it into the output queue
      result.token_memory.push_back(std::move(parsed_token));
      result.output_queue.push_back(result.token_memory.back().get());
    }

    // Token is a variable
    else if (auto variable_spec_iterator =
                 parse_token_from_spec_list(formula, spec.variables);
             variable_spec_iterator != spec.variables.end()) {
      // Put it into the output queue
      result.output_queue.push_back(variable_spec_iterator.base());
    }

    // Token is a function
    else if (auto function_spec_iterator =
                 parse_token_from_spec_list(formula, spec.functions);
             function_spec_iterator != spec.functions.end()) {
      // Push it onto the operator stack
      operator_stack.push_back(function_spec_iterator.base());
    }

    // Token is an operator 'a'
    else if (auto operator_a_spec_iterator =
                 parse_token_from_spec_list(formula, spec.operators);
             operator_a_spec_iterator != spec.operators.end()) {
      operator_t const &operator_a = *operator_a_spec_iterator;

      // while there is an operator 'b' at the top of the operator stack
      // which is not a left parenthesis,
      while (!operator_stack.empty() &&
             operator_stack.back()->visit([&]<typename OperatorBType>(
                                              OperatorBType const
                                                  &operator_b_as_auto) -> bool {
               if constexpr (std::is_same_v<OperatorBType, operator_t>) {
                 operator_t const &operator_b = operator_b_as_auto;
                 // if ('b' has greater precedence than 'a' or
                 // ('a' and 'b' have the same precedence and 'a' is
                 // left-associative))
                 if (operator_b.precedence > operator_a.precedence ||
                     (operator_a.precedence == operator_b.precedence &&
                      operator_a.associativity == left_v)) {
                   return true;
                 }
               } else if constexpr (std::is_same_v<OperatorBType, function_t>) {
                 // or 'b' is a function
                 return true;
               }
               // left parenthesis or lower precedence operator
               return false;
             })) {
        // pop 'b' from the operator stack into the output queue
        result.output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
      }
      // push 'a' onto the operator stack
      operator_stack.push_back(operator_a_spec_iterator.base());
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

/// Flattens an rpn result to a vector of literal_token_t values.
constexpr std::vector<literal_token_t>
flatten_rpn_result(rpn_result_t const &rpn_input) {
  std::vector<literal_token_t> result;
  for (token_base_t const *token_ptr : rpn_input.output_queue) {
    token_ptr->visit(
        [&](auto const &typed_token) { result.push_back({typed_token}); });
  }
  return result;
}

/// Asuming Fun is a constexpr function that returns a std::vector value,
/// eval_as_array will store its contents into an std::array.
template <auto Fun> constexpr auto eval_as_array() {
  constexpr std::size_t Size = Fun().size();
  std::array<typename decltype(Fun())::value_type, Size> res;
  std::ranges::copy(Fun(), res.begin());
  return res;
}

/// Converts an array of variants into a tuple of
/// its value as their underlaying types.
template <auto const &ArrayOfVariants>
constexpr auto array_of_variants_to_tuple() {
  // Storing the size of the array in a constexpr variable.
  constexpr std::size_t Size = ArrayOfVariants.size();

  // Static for-loop on array indexes
  return []<std::size_t... ArrayIndexPack>(
      std::index_sequence<ArrayIndexPack...>) {
    // Making a tuple of the array elements
    return kumi::make_tuple(
        // Unrolling the fold expression into a lambda that extracts the variant
        // elements into values of their actual types
        []<std::size_t UnpackedArrayIndex>(
            std::integral_constant<std::size_t, UnpackedArrayIndex>) {
          // Extracting type index into a constexpr variable
          constexpr std::size_t TypeIndex =
              ArrayOfVariants[UnpackedArrayIndex].index();
          // Getting the underlaying value
          return std::get<TypeIndex>(ArrayOfVariants[UnpackedArrayIndex]);
        }(std::integral_constant<std::size_t, ArrayIndexPack>{})...);
  }
  (std::make_index_sequence<Size>{});
}

/// For each token in RPNStackAsTuple, consume_tokens will call the
template <auto const &RPNStackAsTuple, std::size_t RPNStackIndex = 0>
constexpr auto consume_tokens(auto consumer, auto state) {
  // If no token is left to handle, return the value stack
  if constexpr (constexpr std::size_t RPNStackSize = kumi::size_v<
                    std::remove_cvref_t<decltype(RPNStackAsTuple)>>;
                RPNStackIndex == RPNStackSize) {
    return state;
  }
  // Otherwise, apply stack for given token and recurse on next token
  else if constexpr (RPNStackIndex < RPNStackSize) {
    // Apply current stack and pass front token as a template parameter
    return consume_tokens<RPNStackAsTuple, RPNStackIndex + 1>(
        consumer,
        consumer.template operator()<RPNStackAsTuple, RPNStackIndex>(state));
  }
}

} // namespace shunting_yard
