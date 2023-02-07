#include "shunting-yard/types.hpp"
#include <algorithm>

#include <kumi/tuple.hpp>
#include <shunting-yard/parse_to_rpn.hpp>
#include <shunting-yard/rpn_to_literal.hpp>

constexpr std::vector<shunting_yard::literal_token_t> foo() {
  namespace sy = shunting_yard;

  sy::token_specification_t rubbish_algebra{
      .variables =
          {
              sy::variable_t("pi"),
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
      parse_to_rpn("sin ( max ( 2, 3 ) / 3 * pi ^ 2 )", rubbish_algebra);

  if (!std::is_constant_evaluated()) {
    fmt::print("Result: ");
    for (sy::token_base_t const *current_token_pointer :
         parsing_result.output_queue) {
      fmt::print("{} ", current_token_pointer->text);
    }
    fmt::print("\n");
  }

  std::vector<sy::literal_token_t> flat_parsing_result =
      sy::flatten_rpn_result(parsing_result, rubbish_algebra);

  return flat_parsing_result;
}

/// Takes an RPN stack as tuple and an RPN index that point to a token,
/// and returns a function object
template <auto const &RPNStackAsTuple, std::size_t RPNStackIndex>
struct dispatcher_t {
  /// Takes the values of the stack as a parameter pack
  static constexpr auto function = [](auto... values) constexpr {
    constexpr auto CurrentToken =
        shunting_yard::tuple_implementation::get<RPNStackIndex>(
            RPNStackAsTuple);

    auto res = shunting_yard::tuple_implementation::make_tuple(CurrentToken,
                                                               values...);

    return res;
  };
};

int main() {
  namespace sy = shunting_yard;

  // Variables must be static, and static variables can't be declared in
  // function templates until C++23. C++20 abstractions are undermined by this
  // rule in this use case.
  static constexpr auto rpn_result_array = sy::eval_as_array<&foo>();
  static constexpr auto rpn_result_tuple =
      sy::array_of_variants_to_tuple<rpn_result_array>();

  constexpr auto processed_result =
      sy::consume_tokens<rpn_result_tuple, dispatcher_t>(
          sy::tuple_implementation::make_tuple());
}
