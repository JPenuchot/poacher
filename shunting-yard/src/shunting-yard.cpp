#include <algorithm>

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

int main() {
  namespace sy = shunting_yard;

  // Static qualifier is necessary, still no idea why
  static constexpr auto val = sy::eval_as_array<&foo>();
  constexpr auto val_as_tuple = sy::array_of_variants_to_tuple<val>();
}
