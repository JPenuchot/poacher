#include <algorithm>
#include <utility>

#include <fmt/core.h>

#include <kumi/tuple.hpp>

#include <shunting-yard/parse_to_rpn.hpp>
#include <shunting-yard/rpn_to_literal.hpp>
#include <shunting-yard/types.hpp>

#include <blaze/Blaze.h>

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

  sy::rpn_result_t parsing_result =
      parse_to_rpn("sin ( ( x + 3 ) / 3 * y ^ 2 )", rubbish_algebra);
      // parse_to_rpn("x + y + 2 + y", rubbish_algebra);

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

int main() {
  namespace sy = shunting_yard;

  foo();
  // Starting formula: "sin ( max ( x, 3 ) / 3 * y ^ 2 )"
  // - Current step: "sin ( max ( x, 3 ) / 3 * y ^ 2 )"
  // - Current step: "( max ( x, 3 ) / 3 * y ^ 2 )"
  // - Current step: "max ( x, 3 ) / 3 * y ^ 2 )"
  // - Current step: "( x, 3 ) / 3 * y ^ 2 )"
  // - Current step: "x, 3 ) / 3 * y ^ 2 )"
  // - Current step: "3 ) / 3 * y ^ 2 )"
  // - Current step: ") / 3 * y ^ 2 )"
  // - Current step: "/ 3 * y ^ 2 )"
  // - Current step: "3 * y ^ 2 )"
  // - Current step: "* y ^ 2 )"
  // - Current step: "y ^ 2 )"
  // - Current step: "^ 2 )"
  // - Current step: "2 )"
  // - Current step: ")"
  // Result: x 3 max 3 / y 2 ^ * sin

  // -> ((((x 3 max) 3 /) (y 2 ^) *) sin)

  // Variables must be static, and static variables can't be declared in
  // function templates until C++23. C++20 abstractions are undermined by this
  // rule in this use case.
  static constexpr auto rpn_result_array = sy::eval_as_array<&foo>();
  static constexpr auto rpn_result_tuple =
      sy::array_of_variants_to_tuple<rpn_result_array>();

  blaze::DynamicVector<float> vector_x(16, 1.);
  blaze::DynamicVector<float> vector_y(16, 12.);

  fmt::print("Array: ");
  for (auto const &e : rpn_result_array) {
    fmt::print("{} ", std::visit([](auto ee) { return ee.text; }, e));
  }
  fmt::print("\n");

  fmt::print("Tuple ({}): ", kumi::size_v<decltype(rpn_result_tuple)>);
  kumi::for_each([](auto elmt) { fmt::print("{} ", elmt.text); },
                 rpn_result_tuple);
  fmt::print("\n");

  [[maybe_unused]] auto processed_result = sy::consume_tokens<rpn_result_tuple>(
      [&]<auto const & RPNStackAsTuple, std::size_t RPNStackIndex>(
          auto operand_stack_tuple) constexpr {
        constexpr auto TokenAsAuto = kumi::get<RPNStackIndex>(RPNStackAsTuple);
        constexpr auto TokenKind = sy::get_kind(TokenAsAuto);

        constexpr auto StackSize = kumi::size_v<decltype(operand_stack_tuple)>;

        // Variable dispatch
        if constexpr (TokenKind == sy::variable_v) {
          constexpr sy::literal_variable_t CurrentToken = TokenAsAuto;

          if constexpr (CurrentToken.text == "x") {
            return kumi::cat(operand_stack_tuple, kumi::tie(vector_x));
          } else if constexpr (CurrentToken.text == "y") {
            return kumi::cat(operand_stack_tuple, kumi::tie(vector_y));
          }
        }

        // Function dispatch
        else if constexpr (TokenKind == sy::function_v) {
          constexpr sy::literal_function_t CurrentToken = TokenAsAuto;

          if constexpr (StackSize == 0) {
            return RPNStackIndex;
          }

          // Enpty stack, big problem.
          else if constexpr (CurrentToken.text == "sin") {
            return kumi::tie(
                blaze::sin(kumi::get<StackSize - 1>(operand_stack_tuple)));
          } else if constexpr (CurrentToken.text == "max") {
            return kumi::tie(kumi::apply(
                [](auto const &...operands) { return blaze::max(operands...); },
                operand_stack_tuple));
          }
        }

        // Operator dispatch
        else if constexpr (TokenKind == sy::operator_v) {
          constexpr sy::literal_operator_t CurrentToken = TokenAsAuto;

          auto const &variable_a =
              kumi::get<StackSize - 2>(operand_stack_tuple);
          auto const &variable_b =
              kumi::get<StackSize - 1>(operand_stack_tuple);

          // Popping the 2 last operands to be replaced
          // with binary operation result
          auto head = kumi::pop_back(kumi::pop_back(operand_stack_tuple));

          if constexpr (CurrentToken.text == "+") {
            return kumi::push_back(head, variable_a + variable_b);
          } else if constexpr (CurrentToken.text == "-") {
            return kumi::push_back(head, variable_a - variable_b);
          } else if constexpr (CurrentToken.text == "*") {
            return kumi::push_back(head, variable_a * variable_b);
          } else if constexpr (CurrentToken.text == "/") {
            return kumi::push_back(head, variable_a / variable_b);
          } else if constexpr (CurrentToken.text == "^") {
            return kumi::push_back(head, blaze::pow(variable_a, variable_b));
          }
        }

        // Constant handling
        else if constexpr (TokenKind == sy::constant_v) {
          constexpr sy::literal_constant_t CurrentToken = TokenAsAuto;
          return kumi::push_back(operand_stack_tuple, CurrentToken.value);
        }
      },
      kumi::make_tuple());

  blaze::DynamicVector<float> res;
  res = kumi::get<0>(processed_result); // TODO: fix bad alloc
  for (float f : res) {
    fmt::print("{} ", f);
  }
}
