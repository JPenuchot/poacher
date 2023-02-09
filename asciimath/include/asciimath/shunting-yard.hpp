#pragma once

/// \file
/// Constexpr implementation of Dijkstra's Shunting yard algorithm:
/// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

#include <algorithm>
#include <ranges>
#include <string_view>
#include <variant>
#include <vector>

#include <asciimath/utils.hpp>

namespace asciimath {

//==============================================================================
// Utility functions for text processing

/// Removes whitespaces at the beginning of a string_view
constexpr void trim_whitespaces(std::string_view &sv) {
  sv.remove_prefix(sv.find_first_not_of(" \t\n"));
}

/// Finds an element of list that is a prefix of sv
constexpr auto find_prefix(std::string_view const &sv,
                           std::ranges::range auto const &list) {
  return std::ranges::find_if(list, [&](std::string_view const &element) {
    return sv.starts_with(element);
  });
}

/// Returns true if the beginning of the formula is a number constant
constexpr std::string_view starts_with_number(std::string_view const &sv) {
  std::size_t end_pos = sv.find_first_not_of("0123456789");
  if (end_pos == 0) {
    return {};
  }

  return {sv.begin(), sv.begin() + end_pos};
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

/// Variable identifier
struct variable_t {
  std::string_view identifier;

  constexpr operator std::string_view() const { return identifier; }
};

/// Specification of an operator
struct operator_t {
  enum associativity_t {
    left_v,
    right_v,
  };

  std::string_view token;
  int precedence = 0;
  associativity_t associativity;

  constexpr operator std::string_view() const { return token; }
};

/// Specification of a function
struct function_t {
  std::string_view identifier;

  constexpr operator std::string_view() const { return identifier; }
};

/// Specification of a parenthesis
struct parenthesis_t {
  enum side_t {
    left_v,
    right_v,
  };

  std::string_view token;
  side_t side;

  constexpr operator std::string_view() const { return token; }
};

/// Variant type for any symbol declaration
using symbol_t =
    std::variant<variable_t, operator_t, function_t, parenthesis_t>;

/// Exracts token from any symbol
constexpr std::string_view extract_token(symbol_t const &sym) {
  return std::visit(
      [](auto const &visited_sym) { return std::string_view(visited_sym); },
      sym);
}

/// Grammar specification, ie. all the symbol declarations
using grammar_spec_t = std::vector<symbol_t>;

grammar_spec_t::const_iterator find_prefix(std::string_view sv,
                                           grammar_spec_t const &spec) {
  return std::ranges::find_if(spec, [&](symbol_t const &s) -> bool {
    return sv.starts_with(std::visit(&extract_token, s));
  });
}

/// Shunting Yard algorithm implementation.
/// https://en.wikipedia.org/wiki/Shunting_yard_algorithm
/// The Shunting Yard algorithm takes a formula as an input and converts it into
/// a Reverse Polish Notation (RPN) representation. This representation is
/// harder to read but trivial to process and turn into an AST. The algorithm
/// supports operator precedence and functions of arbitrary arities.

constexpr auto shunting_yard(std::string_view formula,
                             grammar_spec_t const &specs) {
  std::vector<std::string_view> output_queue;
  std::vector<symbol_t> operator_stack;

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
    if (std::string_view num_view = starts_with_number(formula);
        !num_view.empty()) {
      // put it into the output queue
      output_queue.push_back(num_view);
      formula.remove_prefix(num_view.size());
    }

    else if (auto symbol_it = find_prefix(formula, specs);
             symbol_it != specs.end()) {
      std::visit(
          utils::overload_t(
              // Second case: variable
              [&](variable_t const &var) {
                // put it into the output queue
                output_queue.push_back(var);
                formula.remove_prefix(var.identifier.size());
              },
              // - a function:

              [&](function_t const &fun) {
                // push it onto the operator stack
                operator_stack.push_back(fun);
                formula.remove_prefix(fun.identifier.size());
              },
              // - an operator "o1":
              [&](operator_t const &op1) {
                //  while (
                //    there is an operator "o2" other than the left
                //    parenthesis at the top of the operator stack, and
                //    (o2 has greater precedence than o1
                //         or they have the same precedence and o1 is
                //         left-associative)
                //  )
                {
                  {
                    // pop o2 from the operator stack into the output
                    // queue
                  }
                  // push o1 onto the operator stack
                }
              },
              [&](parenthesis_t const &paren) {
                switch (paren.side) {
                  // - a left parenthesis (i.e. "("):
                case parenthesis_t::left_v:
                  // push it onto the operator stack
                  operator_stack.push_back(paren);
                  formula.remove_prefix(paren.token.size());
                  break;
                  // - a right parenthesis (i.e. ")"):
                case parenthesis_t::right_v:

                  //  while the operator at the top of the operator stack
                  //  is not a left parenthesis:
                  while (
                      !operator_stack.empty() &&
                      !(std::holds_alternative<parenthesis_t>(
                            operator_stack.back()) &&
                        std::get<parenthesis_t>(operator_stack.back()).side ==
                            parenthesis_t::side_t::left_v)) {
                    // {assert the operator stack is not empty}
                    if (operator_stack.empty()) {
                      // If the stack runs out without finding a left
                      // parenthesis, then there are mismatched
                      // parentheses.
                      throw; // TODO: Improve constexpr error reporting
                    }

                    // pop the operator from the operator stack into the
                    // output queue
                    output_queue.push_back(
                        extract_token(operator_stack.back()));
                    operator_stack.pop_back();
                  }

                  //  {assert there is a left parenthesis at the top of
                  //  the operator stack} pop the left parenthesis from
                  //  the operator stack and discard it if there is a
                  //  function token at the top of the operator stack,
                  //  then:
                  //    pop the function from the operator stack into the
                  //    output queue
                  break;
                }
              }),
          *symbol_it);
    }

    else if (auto it = find_prefix(formula, specs.right_parenthesis);
             it != specs.right_parenthesis.end()) {

      //  while the operator at the top of the operator stack is not a
      //  left parenthesis:
      for (auto it =
               std::ranges::find(specs.left_parenthesis, operator_stack.back());
           it == specs.left_parenthesis.end();
           it = std::ranges::find(specs.left_parenthesis,

                                  operator_stack.back())) {
        // {assert the operator stack is not empty}
        if (operator_stack.empty()) {
          // If the stack runs out without finding a left parenthesis, then
          // there are mismatched parentheses.
          throw; // TODO: Improve constexpr error reporting
        }

        // pop the operator from the operator stack into the output queue
        output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
      }

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
  while (!operator_stack.empty()) {
    // {assert the operator on top of the stack is not a (left) parenthesis}
    if (auto it =
            std::ranges::find(specs.left_parenthesis, operator_stack.back());
        it != specs.left_parenthesis.end()) {
      // If the operator token on the top of the stack is a parenthesis, then
      // there are mismatched parentheses.
      throw; // TODO: Improve constexpr error reporting
    }

    // pop the operator from the operator stack onto the output queue
    output_queue.push_back(operator_stack.back());
    operator_stack.pop_back();
  }
}

} // namespace asciimath
