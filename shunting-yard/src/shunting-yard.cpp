#include <algorithm>
#include <utility>

#include <fmt/core.h>

#include <kumi/tuple.hpp>

#include <shunting-yard/parse_to_rpn.hpp>
#include <shunting-yard/rpn_to_literal.hpp>
#include <shunting-yard/types.hpp>

#include <blaze/Blaze.h>

/// Parses a given formula to Reverse Polish Notation (RPN).
template <auto const &Formula>
constexpr std::vector<shunting_yard::literal_token_t> parse_rubbish() {
  namespace sy = shunting_yard;

  // Defining various tokens
  sy::token_specification_t rubbish_algebra{
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
      .lparens = {sy::lparen_t("(")},
      .rparens = {sy::rparen_t(")")}};

  sy::rpn_result_t parsing_result = parse_to_rpn(Formula, rubbish_algebra);

  // Printing the result (unless the function is constant evaluated)
  if (!std::is_constant_evaluated()) {
    fmt::print("Result: ");
    for (sy::token_base_t const *current_token_pointer :
         parsing_result.output_queue) {
      fmt::print("{} ", current_token_pointer->text);
    }
    fmt::print("\n");
  }

  return sy::flatten_rpn_result(parsing_result);
}

/// Parses a given formula and generates the corresponsing code
template <auto const &Formula>
auto process_rubbish(auto const &input_x, auto const &input_y) {
  namespace sy = shunting_yard;

  // First transformation into an array
  constexpr static auto rpn_result_array =
      sy::eval_as_array<[]() constexpr { return parse_rubbish<Formula>(); }>();

  // Second transformation into a tuple
  constexpr static auto rpn_result_tuple =
      sy::array_of_variants_to_tuple<rpn_result_array>();

  // Defining actions for each token
  auto token_processor = [&]<auto const & RPNStackAsTuple,
                             std::size_t RPNStackIndex>(
      auto operand_stack_tuple) constexpr {
    constexpr auto TokenAsAuto = kumi::get<RPNStackIndex>(RPNStackAsTuple);

    constexpr sy::token_kind_t TokenKind = sy::get_kind(TokenAsAuto);

    // Constant handling
    if constexpr (TokenKind == sy::constant_v) {
      constexpr sy::literal_constant_t CurrentToken = TokenAsAuto;
      return kumi::push_front(
          operand_stack_tuple,
          [constant = CurrentToken.value]() { return constant; });
    }

    // Variable dispatch
    else if constexpr (TokenKind == sy::variable_v) {
      constexpr sy::literal_variable_t CurrentToken = TokenAsAuto;

      if constexpr (CurrentToken.text == "x") {
        return kumi::push_front(
            operand_stack_tuple, [&input_x]() -> auto const & {
              return input_x;
            });
      } else if constexpr (CurrentToken.text == "y") {
        return kumi::push_front(
            operand_stack_tuple, [&input_y]() -> auto const & {
              return input_y;
            });
      }
    }

    // Function dispatch
    else if constexpr (TokenKind == sy::function_v) {
      constexpr sy::literal_function_t CurrentToken = TokenAsAuto;

      if constexpr (CurrentToken.text == "sin") {
        auto const &operand = kumi::get<0>(operand_stack_tuple);
        auto head = kumi::pop_front(operand_stack_tuple);

        return kumi::push_front(head, [operand]() { return sin(operand()); });
      }

      else if constexpr (CurrentToken.text == "max") {
        auto const &operand_a = kumi::get<1>(operand_stack_tuple);
        auto const &operand_b = kumi::get<0>(operand_stack_tuple);
        auto head = kumi::pop_front(kumi::pop_front(operand_stack_tuple));

        return kumi::push_front(head, [operand_a, operand_b]() {
          return max(operand_a(), operand_b());
        });
      }
    }

    // Operator dispatch
    else if constexpr (TokenKind == sy::operator_v) {
      constexpr sy::literal_operator_t CurrentToken = TokenAsAuto;

      auto const &operand_a = kumi::get<1>(operand_stack_tuple);
      auto const &operand_b = kumi::get<0>(operand_stack_tuple);

      // Popping the 2 last operands to be replaced
      // with binary operation result
      auto head = kumi::pop_front(kumi::pop_front(operand_stack_tuple));

      if constexpr (CurrentToken.text == "+") {
        return kumi::push_front(head, [operand_a, operand_b]() {
          return operand_a() + operand_b();
        });
      } else if constexpr (CurrentToken.text == "-") {
        return kumi::push_front(head, [operand_a, operand_b]() {
          return operand_a() - operand_b();
        });
      } else if constexpr (CurrentToken.text == "*") {
        return kumi::push_front(head, [operand_a, operand_b]() {
          return operand_a() * operand_b();
        });
      } else if constexpr (CurrentToken.text == "/") {
        return kumi::push_front(head, [operand_a, operand_b]() {
          return operand_a() / operand_b();
        });
      } else if constexpr (CurrentToken.text == "^") {
        return kumi::push_front(head, [operand_a, operand_b]() {
          return blaze::pow(operand_a(), operand_b());
        });
      }
    }
  };

  // Processing the RPN representation of the formula.
  // The result should be a single lambda in a tuple.
  auto [result] =
      sy::consume_tokens<rpn_result_tuple>(token_processor, kumi::make_tuple());
  return result();
}

int main() {
  static constexpr auto formula = "sin((x + 3) / 3 * y ^ 2)";

  // Runtime parsing prints parsing steps
  parse_rubbish<formula>();

  // Input vectors
  constexpr std::size_t vec_size = 16;
  blaze::DynamicVector<float> vector_x(vec_size, 1.);
  blaze::DynamicVector<float> vector_y(vec_size, 12.);

  // Processing the formula into an expression template, and printing it.
  for (float const e : process_rubbish<formula>(vector_x, vector_y)) {
    fmt::print("{} ", e);
  }
  fmt::print("\n");
}
