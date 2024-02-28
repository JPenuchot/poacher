#pragma once

#include <shunting_yard.hpp>

#include <kumi/tuple.hpp>

#include <fmt/core.h>

#include <vector>

namespace tiny_math_language {

/// Parses a given formula to Reverse Polish Notation
/// (RPN).
constexpr shunting_yard::parse_result_t
parse(std::string_view const &formula) {
  namespace sy = shunting_yard;

  // Defining various tokens
  sy::token_specification_t tiny_math_language_spec{
      .variables =
          {
              sy::variable_t("x"),
              sy::variable_t("y"),
          },
      .functions =
          {
              sy::function_t("sin"),
              sy::function_t("max"),
          },
      .operators =
          {
              sy::operator_t("+", sy::left_v, 10),
              sy::operator_t("-", sy::left_v, 10),
              sy::operator_t("*", sy::left_v, 20),
              sy::operator_t("/", sy::left_v, 20),
              sy::operator_t("^", sy::right_v, 30),
          },
      .lparens = {sy::lparen_t("("),
                  sy::lparen_t("{")},
      .rparens = {sy::rparen_t(")"),
                  sy::rparen_t("}")}};

  // Running the Shunting yard algorithm with our
  // token specification
  sy::parse_result_t parsing_result =
      parse_to_rpn(formula, tiny_math_language_spec);

  // Printing the result (unless the function is
  // constant evaluated)
  if !consteval {
    fmt::print("Result: ");
    for (sy::token_t const &current_token :
         parsing_result) {
      fmt::print("{} ",
                 std::visit(
                     [](auto const &visited_token) {
                       return visited_token.text;
                     },
                     current_token));
    }
    fmt::print("\n");
  }

  return parsing_result;
}

/// Parses a given formula and generates the
/// corresponsing code
template <auto const &Formula>
constexpr auto codegen() {
  namespace sy = shunting_yard;

  // First transformation into an array
  constexpr static auto rpn_result_array =
      sy::eval_as_array<[]() constexpr {
        return parse(Formula);
      }>();

  // Defining actions for each token
  auto token_processor =
      [&]<auto const & RPNStackAsArray,
          std::size_t RPNStackIndex>(
          auto operand_stack_tuple) constexpr {
        constexpr std::size_t TypeId =
            RPNStackAsArray[RPNStackIndex].index();
        constexpr auto TokenAsAuto = std::get<TypeId>(
            RPNStackAsArray[RPNStackIndex]);

        constexpr sy::token_kind_t TokenKind =
            sy::get_kind(TokenAsAuto);

        // Constant handling
        if constexpr (TokenKind == sy::constant_v) {
          constexpr sy::constant_t CurrentToken =
              TokenAsAuto;
          return kumi::push_front(
              operand_stack_tuple,
              [constant = CurrentToken.value](
                  auto const &, auto const &) {
                return constant;
              });
        }

        // Variable dispatch
        else if constexpr (TokenKind ==
                           sy::variable_v) {
          constexpr sy::variable_t CurrentToken =
              TokenAsAuto;

          if constexpr (CurrentToken.text == "x") {
            return kumi::push_front(
                operand_stack_tuple,
                [](auto const &input_x,
                   auto const &) -> auto const & {
                  return input_x;
                });
          } else if constexpr (CurrentToken.text ==
                               "y") {
            return kumi::push_front(
                operand_stack_tuple,
                [](auto const &, auto const &input_y)
                    -> auto const & {
                  return input_y;
                });
          }
        }

        // Function dispatch
        else if constexpr (TokenKind ==
                           sy::function_v) {
          constexpr sy::function_t CurrentToken =
              TokenAsAuto;

          if constexpr (CurrentToken.text == "sin") {
            auto const &operand =
                kumi::get<0>(operand_stack_tuple);
            auto head =
                kumi::pop_front(operand_stack_tuple);

            return kumi::push_front(
                head, [operand](auto const &input_x,
                                auto const &input_y) {
                  return sin(
                      operand(input_x, input_y));
                });
          }

          else if constexpr (CurrentToken.text ==
                             "max") {
            auto const &operand_a =
                kumi::get<1>(operand_stack_tuple);
            auto const &operand_b =
                kumi::get<0>(operand_stack_tuple);
            auto head = kumi::pop_front(
                kumi::pop_front(operand_stack_tuple));

            return kumi::push_front(
                head, [operand_a, operand_b](
                          auto const &input_x,
                          auto const &input_y) {
                  return max(
                      operand_a(input_x, input_y),
                      operand_b(input_x, input_y));
                });
          }
        }

        // Operator dispatch
        else if constexpr (TokenKind ==
                           sy::operator_v) {
          constexpr sy::operator_t CurrentToken =
              TokenAsAuto;

          auto const &operand_a =
              kumi::get<1>(operand_stack_tuple);
          auto const &operand_b =
              kumi::get<0>(operand_stack_tuple);

          // Popping the 2 last operands to be
          // replaced with binary operation result
          auto head = kumi::pop_front(
              kumi::pop_front(operand_stack_tuple));

          if constexpr (CurrentToken.text == "+") {
            return kumi::push_front(
                head, [operand_a, operand_b](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_a(input_x, input_y) +
                         operand_b(input_x, input_y);
                });
          } else if constexpr (CurrentToken.text ==
                               "-") {
            return kumi::push_front(
                head, [operand_a, operand_b](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_a(input_x, input_y) -
                         operand_b(input_x, input_y);
                });
          } else if constexpr (CurrentToken.text ==
                               "*") {
            return kumi::push_front(
                head, [operand_a, operand_b](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_a(input_x, input_y) *
                         operand_b(input_x, input_y);
                });
          } else if constexpr (CurrentToken.text ==
                               "/") {
            return kumi::push_front(
                head, [operand_a, operand_b](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_a(input_x, input_y) /
                         operand_b(input_x, input_y);
                });
          } else if constexpr (CurrentToken.text ==
                               "^") {
            return kumi::push_front(
                head, [operand_a, operand_b](
                          auto const &input_x,
                          auto const &input_y) {
                  return pow(
                      operand_a(input_x, input_y),
                      operand_b(input_x, input_y));
                });
          }
        }
      };

  // Processing the RPN representation of the formula.
  // The result should be a single lambda in a tuple.
  auto [result] =
      sy::consume_tokens<rpn_result_array>(
          token_processor, kumi::make_tuple());
  return result;
}

} // namespace tiny_math_language
