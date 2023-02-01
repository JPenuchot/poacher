#pragma once

// Implementation based on:
// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

#include <iterator>
#include <ranges>
#include <string_view>
#include <vector>

#include <fmt/core.h>

#include <shunting-yard/types.hpp>

namespace shunting_yard {

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
constexpr unique_ptr_selector::unique_ptr<token_base_t>
parse_number(std::string_view &text) {
  // Checking for presence of a digit
  std::size_t find_result = text.find_first_not_of("0123456789");

  if (find_result == 0) {
    return unique_ptr_selector::make_unique<failure_t>();
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
  return unique_ptr_selector::make_unique<constant_t>(result, number_view);
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
    if (unique_ptr_selector::unique_ptr<token_base_t> parsed_token =
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

} // namespace shunting_yard
