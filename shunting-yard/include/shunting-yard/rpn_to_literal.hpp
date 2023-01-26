#pragma once

#include <array>
#include <concepts>
#include <type_traits>
#include <variant>

#include <shunting-yard/parse_to_rpn.hpp>
#include <shunting-yard/types.hpp>

namespace shunting_yard {

/// Literal version for token base type (see token_t)
struct literal_token_base_t {
  std::string_view text;
};

template <typename T>
concept is_literal_token_c = requires(T e) {
                               {
                                 e.text
                                 } -> std::convertible_to<std::string_view>;
                             };

/// Literal version for empty token for parsing failure management
struct literal_failure_t : literal_token_base_t {
  constexpr literal_failure_t() : literal_token_base_t{{}} {}

  constexpr literal_failure_t(failure_t const &non_literal)
      : literal_failure_t() {}
};

/// Literal version for variable spec type
struct literal_variable_t : literal_token_base_t {
  constexpr literal_variable_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  constexpr literal_variable_t(variable_t const &non_literal)
      : literal_variable_t(non_literal.text) {}
};

/// Literal version for function spec type
struct literal_function_t : literal_token_base_t {
  constexpr literal_function_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  constexpr literal_function_t(function_t const &non_literal)
      : literal_function_t(non_literal.text) {}
};

/// Literal version for operator spec type
struct literal_operator_t : literal_token_base_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr literal_operator_t(std::string_view identifier,
                               operator_associativity_t associativity_,
                               unsigned precedence_)
      : literal_token_base_t{identifier}, associativity(associativity_),
        precedence(precedence_) {}

  constexpr literal_operator_t(operator_t const &non_literal)
      : literal_operator_t(non_literal.text, non_literal.associativity,
                           non_literal.precedence) {}
};

/// Literal version for left parenthesis spec type
struct literal_lparen_t : literal_token_base_t {
  constexpr literal_lparen_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  constexpr literal_lparen_t(lparen_t const &non_literal)
      : literal_lparen_t(non_literal.text) {}
};

/// Literal version for right parenthesis spec type
struct literal_rparen_t : literal_token_base_t {
  constexpr literal_rparen_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  constexpr literal_rparen_t(rparen_t const &non_literal)
      : literal_rparen_t(non_literal.text) {}
};

/// Constant (unsigned integer)
struct literal_constant_t : literal_token_base_t {
  unsigned value;
  constexpr literal_constant_t(unsigned value_, std::string_view number)
      : literal_token_base_t{number}, value(value_) {}

  constexpr literal_constant_t(constant_t const &non_literal)
      : literal_constant_t(non_literal.value, non_literal.text) {}
};

/// Literal generic type for a token.
using literal_token_t =
    std::variant<literal_failure_t, literal_variable_t, literal_function_t,
                 literal_operator_t, literal_lparen_t, literal_rparen_t,
                 literal_constant_t>;

// Sanity check
namespace _test {
constexpr literal_token_t test_literal_token(literal_constant_t(1, "one"));
}

/// Flattens an rpn result to a vector of literal_token_t values.
constexpr std::vector<literal_token_t>
flatten_rpn_result(rpn_result_t const &rpn_input,
                   token_specification_t const &spec) {
  std::vector<literal_token_t> result;
  for (token_t const *token_ptr : rpn_input.output_queue) {
    token_ptr->visit(
        [&](auto const &typed_token) { result.push_back({typed_token}); });
  }
  return result;
}

/// Asuming Fun is a constexpr function that returns a std::vector value,
/// eval_as_array will store its contents into an std::array.
template <auto Fun> constexpr auto eval_as_array() {
  std::array<typename decltype(Fun())::value_type, Fun().size()> res;
  std::ranges::copy(Fun(), res.begin());
  return res;
}

} // namespace shunting_yard
