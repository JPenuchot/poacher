#include <algorithm>
#include <utility>

#include <fmt/core.h>

#include <kumi/tuple.hpp>

#include <shunting-yard/parse_to_rpn.hpp>
#include <shunting-yard/rpn_to_literal.hpp>
#include <shunting-yard/types.hpp>

#include <blaze/Blaze.h>

template <auto const &Formula>
constexpr std::vector<shunting_yard::literal_token_t> foo() {
  namespace sy = shunting_yard;

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

  if (!std::is_constant_evaluated()) {
    fmt::print("Result: ");
    for (sy::token_base_t const *current_token_pointer :
         parsing_result.output_queue) {
      fmt::print("{} ", current_token_pointer->text);
    }
    fmt::print("\n");
  }

  std::vector<sy::literal_token_t> flat_parsing_result =
      sy::flatten_rpn_result(parsing_result);

  return flat_parsing_result;
}

template <auto const &Formula> auto process_rubbish() {
  namespace sy = shunting_yard;

  constexpr static auto rpn_result_array =
      sy::eval_as_array<[]() constexpr { return foo<Formula>(); }>();
  constexpr static auto rpn_result_tuple =
      sy::array_of_variants_to_tuple<rpn_result_array>();

  blaze::DynamicVector<float> vector_x(16, 1.);
  blaze::DynamicVector<float> vector_y(16, 12.);

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
            operand_stack_tuple, [&vector_x]() -> auto const & {
              return vector_x;
            });
      } else if constexpr (CurrentToken.text == "y") {
        return kumi::push_front(
            operand_stack_tuple, [&vector_y]() -> auto const & {
              return vector_y;
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

  auto expression_template_generator =
      sy::consume_tokens<rpn_result_tuple>(token_processor, kumi::make_tuple());

  return kumi::front(expression_template_generator)();
}

int main() {
  static constexpr auto formula = "sin ( ( x + 3 ) / 3 * y ^ 2 )";

  // Runtime parsing prints parsing steps
  foo<formula>();

  // Input vectors
  blaze::DynamicVector<float> vector_x(16, 1.);
  blaze::DynamicVector<float> vector_y(16, 12.);

  // Making an expression template generator. This will output a tuple whose
  // only element is a function that generates a Blaze expression template
  // corresponding to the parsed formula.
  for (float const e : process_rubbish<formula>()) {
    fmt::print("{} ", e);
  }
  fmt::print("\n");
}
