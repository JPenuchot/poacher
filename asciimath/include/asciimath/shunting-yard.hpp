#pragma once

/// \file
/// Constexpr implementation of Dijkstra's Shunting yard algorithm:
/// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

#include <algorithm>
#include <ranges>
#include <string_view>
#include <variant>
#include <vector>

namespace asciimath {

//==============================================================================
// Utility functions for text processing

/// Removes whitespaces at the beginning of a string_view
constexpr void trim_whitespaces(std::string_view &sv) {
  sv.remove_prefix(sv.find_first_not_of(" \t\n"));
}

/// Returns true if sv starts with any element contained in list.
constexpr bool starts_with_any_of(std::string_view const &sv,
                                  std::ranges::range auto const &list) {
  return std::ranges::any_of(list, [&](std::string_view const &element) {
    return sv.starts_with(element);
  });
}

/// Returns true if the beginning of the formula is a number constant
constexpr bool starts_with_number(std::string_view const &sv) {
  // TODO: Implement
  return false;
}

constexpr std::string_view
parse_token_from_list(std::string_view &sv,
                      std::ranges::range auto const &list) {
  if (auto it = std::find(list.begin(), list.end(),
                          [&](std::string_view const &token) -> bool {
                            return sv.starts_with(token);
                          });
      it != list.end()) {
    sv.remove_prefix(it->size());
    return *it;
  }
  return {};
}

//==============================================================================
// Functions related

using operator_t = std::string_view;

using output_element_t = std::variant<operator_t>;

/// Specification of an algebra. This structure is used to define variable
/// identifiers, function identifiers, infix operators, and parenthesis.
struct algebra_specs_t {
  std::vector<std::string_view> variable_identifiers;
  std::vector<std::string_view> function_identifiers;

  std::vector<std::string_view> o1_operators;
  std::vector<std::string_view> o2_operators;

  std::vector<std::string_view> left_parenthesis = {"(", "[", "{"};
  std::vector<std::string_view> right_parenthesis = {")", "]", "}"};
};

/// Validates algebra specs. If the specs are incorrect, the program will stop.
constexpr void validate_specs(algebra_specs_t const &specs) {
  // TODO
}

/// Shunting Yard algorithm implementation.
/// https://en.wikipedia.org/wiki/Shunting_yard_algorithm
/// The Shunting Yard algorithm takes a formula as an input and converts it into
/// a Reverse Polish Notation (RPN) representation. This representation is
/// harder to read but trivial to process and turn into an AST. The algorithm
/// supports operator precedence and functions of arbitrary arities.

constexpr auto shunting_yard(std::string_view formula,
                             algebra_specs_t const &specs) {
  validate_specs(specs);

  std::vector<std::string_view> output_queue;
  std::vector<std::string_view> operator_stack;

  // Pseudocode:

  // The functions referred to in this algorithm are simple single argument
  // functions such as sine, inverse or factorial.

  // This implementation does not implement composite functions,
  // functions with a variable number of arguments, or unary operators.

  //  while there are tokens to be read:
  while (!formula.empty()) {
    trim_whitespaces(formula);

    // read a token, if the token is:

    // - a number:

    // First case: number constant
    if (starts_with_number(formula)) {
      // put it into the output queue
    }

    // Second case: variable
    else if (starts_with_any_of(formula, specs.variable_identifiers)) {
      // put it into the output queue
    }

    // - a function:

    else if (starts_with_any_of(formula, specs.function_identifiers)) {
      // push it onto the operator stack
    }

    // - an o1 operator:

    else if (starts_with_any_of(formula, specs.o1_operators)) {
      //  while (
      //    there is an o2 operator other than the left parenthesis at
      //    the top of the operator stack, and (o2 has greater
      //    precedence than o1 or they have the same precedence and o1
      //    is left-associative)
      //  )
      {
        // pop o2 from the operator stack into the output queue
      }
      // push o1 onto the operator stack

    }

    // - a left parenthesis (i.e. "("):

    else if (starts_with_any_of(formula, specs.left_parenthesis)) {
      // push it onto the operator stack
    }

    // - a right parenthesis (i.e. ")"):

    else if (starts_with_any_of(formula, specs.right_parenthesis)) {
      //  while the operator at the top of the operator stack is not a
      //  left parenthesis:
      //    {assert the operator stack is not empty}
      //    // If the stack runs out without finding a left
      //    // parenthesis, then there are mismatched parentheses.
      //    pop the operator from the operator stack into the output queue
      //  {assert there is a left parenthesis at the top of the operator stack}
      //  pop the left parenthesis from the operator stack and discard
      //  it if there is a function token at the top of the operator
      //  stack, then:
      //    pop the function from the operator stack into the output queue
    }
  }

  // After the while loop, pop the remaining items from the operator stack into
  // the output queue.

  //  while there are tokens on the operator stack:
  //      // If the operator token on the top of the stack is a parenthesis,
  //      // then there are mismatched parentheses.
  //      {assert the operator on top of the stack is not a (left) parenthesis}
  //      pop the operator from the operator stack onto the output queue
}

} // namespace asciimath
