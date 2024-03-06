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

  // Debug output, only at runtime
  if !consteval {
    fmt::print("Parsed output: ");
    for (sy::token_variant_t const &current_token :
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

/// Parses a given formula and generates
/// the corresponsing code
template <auto const &Formula>
constexpr auto codegen() {
  namespace sy = shunting_yard;

  // First transformation into an array
  constexpr static auto rpn_result_array =
      sy::eval_as_array<[]() constexpr {
        return parse(Formula);
      }>();

  /// Updates the stack depending on
  /// the token being read.
  auto process_token =
      [&]<auto const & RPNStackAsArray,
          std::size_t RPNStackIndex>(
          auto operand_stack_tuple) constexpr {
        constexpr sy::token_variant_t TokenVariant =
            RPNStackAsArray[RPNStackIndex];
        constexpr std::size_t TypeId =
            TokenVariant.index();

        // Extracting token variant
        // into its underlying type
        constexpr auto Token =
            std::get<TypeId>(TokenVariant);

        // Constant handling
        if constexpr (constexpr sy::token_kind_t
                          TokenKind =
                              sy::get_kind(Token);
                      TokenKind == sy::constant_v) {
          constexpr sy::constant_t Constant = Token;
          // Push the constant operand at the front
          return kumi::push_front(
              operand_stack_tuple,
              [constant = Constant.value](
                  auto const &, auto const &) {
                return constant;
              });
        }

        // Variable handling
        else if constexpr (TokenKind ==
                           sy::variable_v) {
          constexpr sy::variable_t Variable = Token;

          if constexpr (Variable.text == "x") {
            return kumi::push_front(
                operand_stack_tuple,
                [](auto const &input_x,
                   auto const &) -> auto const & {
                  return input_x;
                });
          } else if constexpr (Variable.text == "y") {
            return kumi::push_front(
                operand_stack_tuple,
                [](auto const &, auto const &input_y)
                    -> auto const & {
                  return input_y;
                });
          }
        }

        // Function handling
        else if constexpr (TokenKind ==
                           sy::function_v) {
          constexpr sy::function_t Function = Token;

          if constexpr (Function.text == "sin") {
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

          else if constexpr (Function.text == "max") {
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

        // Operator handling
        else if constexpr (TokenKind ==
                           sy::operator_v) {
          constexpr sy::operator_t Operator = Token;

          // Pop left and right operands
          // from the top of the stack
          auto const &operand_l =
              kumi::get<1>(operand_stack_tuple);
          auto const &operand_r =
              kumi::get<0>(operand_stack_tuple);
          auto tail = kumi::pop_front(
              kumi::pop_front(operand_stack_tuple));

          if constexpr (Operator.text == "+") {
            // Generate and push the plus operand
            // at the front of the stack
            return kumi::push_front(
                tail, [operand_l, operand_r](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_l(input_x, input_y) +
                         operand_r(input_x, input_y);
                });
          } else if constexpr (Operator.text == "-") {
            return kumi::push_front(
                tail, [operand_l, operand_r](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_l(input_x, input_y) -
                         operand_r(input_x, input_y);
                });
          } else if constexpr (Operator.text == "*") {
            return kumi::push_front(
                tail, [operand_l, operand_r](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_l(input_x, input_y) *
                         operand_r(input_x, input_y);
                });
          } else if constexpr (Operator.text == "/") {
            return kumi::push_front(
                tail, [operand_l, operand_r](
                          auto const &input_x,
                          auto const &input_y) {
                  return operand_l(input_x, input_y) /
                         operand_r(input_x, input_y);
                });
          } else if constexpr (Operator.text == "^") {
            return kumi::push_front(
                tail, [operand_l, operand_r](
                          auto const &input_x,
                          auto const &input_y) {
                  return pow(
                      operand_l(input_x, input_y),
                      operand_r(input_x, input_y));
                });
          }
        }
      };

  // Processing the RPN representation of the formula.
  // The result should be a single lambda in a tuple.
  auto [result] =
      sy::consume_tokens<rpn_result_array>(
          process_token, kumi::make_tuple());
  return result;
}

} // namespace tiny_math_language
