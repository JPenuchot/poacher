#pragma once

// Implementation based on:
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

#include <algorithm>
#include <iterator>
#include <ranges>
#include <string_view>
#include <variant>
#include <vector>

#include <kumi/tuple.hpp>

#include <fmt/core.h>

namespace shunting_yard {

/// Operator associativity
enum operator_associativity_t { left_v, right_v };

/// Literal version for token base type (see token_t)
struct token_base_t {
  std::string_view text;
};

/// Literal version for empty token for parsing failure management
struct failure_t : token_base_t {
  constexpr failure_t() : token_base_t{{}} {}
};

/// Literal version for variable spec type
struct variable_t : token_base_t {
  constexpr variable_t(std::string_view identifier)
      : token_base_t{identifier} {}
};

/// Literal version for function spec type
struct function_t : token_base_t {
  constexpr function_t(std::string_view identifier)
      : token_base_t{identifier} {}
};

/// Literal version for operator spec type
struct operator_t : token_base_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr operator_t(std::string_view identifier,
                       operator_associativity_t associativity_,
                       unsigned precedence_)
      : token_base_t{identifier}, associativity(associativity_),
        precedence(precedence_) {}
};

/// Literal version for left parenthesis spec type
struct lparen_t : token_base_t {
  constexpr lparen_t(std::string_view identifier) : token_base_t{identifier} {}
};

/// Literal version for right parenthesis spec type
struct rparen_t : token_base_t {
  constexpr rparen_t(std::string_view identifier) : token_base_t{identifier} {}
};

/// Constant (unsigned integer)
struct constant_t : token_base_t {
  unsigned value;
  constexpr constant_t(unsigned value_, std::string_view number)
      : token_base_t{number}, value(value_) {}
};

/// Literal generic type for a token.
using token_t = std::variant<failure_t, variable_t, function_t, operator_t,
                             lparen_t, rparen_t, constant_t>;

// Sanity check
namespace _test {
constexpr token_t test_literal_token(constant_t(1, "one"));
}

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

/// Helper kind getter for literal_failure_t.
constexpr token_kind_t get_kind(failure_t const &) { return failure_v; }
/// Helper kind getter for literal_variable_t.
constexpr token_kind_t get_kind(variable_t const &) { return variable_v; }
/// Helper kind getter for literal_function_t.
constexpr token_kind_t get_kind(function_t const &) { return function_v; }
/// Helper kind getter for literal_operator_t.
constexpr token_kind_t get_kind(operator_t const &) { return operator_v; }
/// Helper kind getter for literal_lparen_t.
constexpr token_kind_t get_kind(lparen_t const &) { return lparen_v; }
/// Helper kind getter for literal_rparen_t.
constexpr token_kind_t get_kind(rparen_t const &) { return rparen_v; }
/// Helper kind getter for literal_constant_t.
constexpr token_kind_t get_kind(constant_t const &) { return constant_v; }

/// Definition of the various tokens for an algebra: variable identifiers,
/// function identifiers, infix operators, and parenthesis.
struct token_specification_t {
  std::vector<variable_t> variables;
  std::vector<function_t> functions;
  std::vector<operator_t> operators;
  std::vector<lparen_t> lparens;
  std::vector<rparen_t> rparens;
};

/// Represents the parsing result of parse_formula.
using parse_result_t = std::vector<token_t>;

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
      std::ranges::find_if(token_list, [&](token_t const &token) {
        return std::visit(
            [&](auto const &visited_token) -> bool {
              return formula.starts_with(visited_token.text);
            },
            token);
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
constexpr token_t parse_number(std::string_view &text) {
  // Checking for presence of a digit
  std::size_t find_result = text.find_first_not_of("0123456789");

  if (find_result == 0) {
    return failure_t();
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
  return constant_t(result, number_view);
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
parse_result_t constexpr parse_to_rpn(std::string_view formula,
                                      token_specification_t const &spec) {
  // The functions referred to in this algorithm are simple single argument
  // functions such as sine, inverse or factorial.

  // This implementation does not implement composite functions, functions
  // with a variable number of arguments, or unary operators.

  parse_result_t output_queue;
  std::vector<token_t> operator_stack;

  if !consteval {
    fmt::print("Starting formula: \"{}\"\n", formula);
  }

  // There are tokens to be read
  while (trim_formula(formula), !formula.empty()) {
    // Debug logs
    if !consteval {
      fmt::print("- Remaining: \"{}\"\n", formula);

      fmt::print("  Output queue: ");
      for (token_t const &current_token : output_queue) {
        fmt::print("{} ", std::visit(
                              [](auto const &visited_token) {
                                return visited_token.text;
                              },
                              current_token));
      }
      fmt::print("\n");

      fmt::print("  Operator stack: ");
      for (token_t const &current_token : operator_stack) {
        fmt::print("{} ", std::visit(
                              [](auto const &visited_token) {
                                return visited_token.text;
                              },
                              current_token));
      }
      fmt::print("\n");
    }
    // read a token

    // Token is a number constant
    if (token_t parsed_token = parse_number(formula);
        std::holds_alternative<constant_t>(parsed_token)) {
      if !consteval {
        fmt::print("Reading number\n");
      }
      // Put it into the output queue
      output_queue.push_back(parsed_token);
    }

    // Token is a variable
    else if (auto variable_spec_iterator =
                 parse_token_from_spec_list(formula, spec.variables);
             variable_spec_iterator != spec.variables.end()) {
      if !consteval {
        fmt::print("Reading variable\n");
      }
      // Put it into the output queue
      output_queue.push_back(*variable_spec_iterator);
    }

    // Token is a function
    else if (auto function_spec_iterator =
                 parse_token_from_spec_list(formula, spec.functions);
             function_spec_iterator != spec.functions.end()) {
      if !consteval {
        fmt::print("Reading function\n");
      }
      // Push it onto the operator stack
      operator_stack.push_back(*function_spec_iterator);
    }

    // Token is an operator 'a'
    else if (auto operator_a_spec_iterator =
                 parse_token_from_spec_list(formula, spec.operators);
             operator_a_spec_iterator != spec.operators.end()) {
      if !consteval {
        fmt::print("Reading operator\n");
      }

      operator_t const &operator_a = *operator_a_spec_iterator;

      // while there is an operator 'b' at the top of the operator stack
      // which is not a left parenthesis,
      while (!operator_stack.empty() &&
             std::visit(
                 [&]<typename BType>(BType const &operator_b_as_auto) -> bool {
                   if constexpr (std::is_same_v<BType, operator_t>) {
                     operator_t const &operator_b = operator_b_as_auto;
                     // if ('b' has greater precedence than 'a' or
                     // ('a' and 'b' have the same precedence and 'a' is
                     // left-associative))
                     if (operator_b.precedence > operator_a.precedence ||
                         (operator_a.precedence == operator_b.precedence &&
                          operator_a.associativity == left_v)) {
                       return true;
                     }
                   } else if constexpr (std::is_same_v<BType, function_t>) {
                     // or 'b' is a function
                     return true;
                   }
                   // left parenthesis or lower precedence operator
                   return false;
                 },
                 operator_stack.back())) {
        // pop 'b' from the operator stack into the output queue
        output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
      }
      // push 'a' onto the operator stack
      operator_stack.push_back(*operator_a_spec_iterator);
    }

    // Token is a left parenthesis (i.e. "(")
    else if (auto lparen_token_iterator =
                 parse_token_from_spec_list(formula, spec.lparens);
             lparen_token_iterator != spec.lparens.end()) {
      if !consteval {
        fmt::print("Reading lparen\n");
      }
      // push it onto the operator stack
      operator_stack.push_back(*lparen_token_iterator);
    }

    // Token is a right parenthesis (i.e. ")")
    else if (auto rparen_token_iterator =
                 parse_token_from_spec_list(formula, spec.rparens);
             rparen_token_iterator != spec.rparens.end()) {
      if !consteval {
        fmt::print("Reading rparen\n");
      }
      // the operator at the top of the operator stack is not a left parenthesis
      while (operator_stack.empty() ||
             !std::holds_alternative<lparen_t>(operator_stack.back())) {

        // {assert the operator stack is not empty}
        if (operator_stack.empty()) {
          // If the stack runs out without finding a left parenthesis, then
          // there are mismatched parentheses.
          fmt::print("Parenthesis mismatch.\n");
          throw;
        }
        // pop the operator from the operator stack into the output queue
        output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
      }

      // {assert there is a left parenthesis at the top of the operator stack}
      if (operator_stack.empty() ||
          !std::holds_alternative<lparen_t>(operator_stack.back())) {
        throw;
      }

      // pop the left parenthesis from the operator stack and discard it
      operator_stack.pop_back();

      // there is a function token at the top of the operator stack
      if (!operator_stack.empty() &&
          std::holds_alternative<function_t>(operator_stack.back())) {
        // pop the function from the operator stack into the output queue
        output_queue.push_back(operator_stack.back());
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
    if (std::holds_alternative<lparen_t>(operator_stack.back())) {
      throw;
    }

    // pop the operator from the operator stack onto the output queue
    output_queue.push_back(operator_stack.back());
    operator_stack.pop_back();
  }

  return output_queue;
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
