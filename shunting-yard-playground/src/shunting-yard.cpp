#include <shunting-yard/shunting-yard.hpp>

constexpr bool foo() {
  namespace sy = shunting_yard;

  sy::grammar_spec_t rubbish_algebra{
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

  sy::shunting_yard_result_t parsing_result =
      // parse_formula("3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3", rubbish_algebra);
      parse_formula("sin ( max ( 2, 3 ) / 3 * pi ^ 2 )", rubbish_algebra);

  if (!std::is_constant_evaluated()) {
    fmt::print("Result: ");
    for (sy::token_t const *current_token_pointer :
         parsing_result.output_queue) {
      fmt::print("{} ", current_token_pointer->text);
    }
    fmt::print("\n");
  }

  return true;
}

int main() {
  static_assert(foo());
  foo();
}
