#pragma once

#include <string_view>
#include <variant>
#include <vector>

#include <cest/memory.hpp>

namespace shunting_yard {

/// unique_ptr implementation namespace selector
namespace unique_ptr_selector = cest;

/// Token kind to match with token types
enum token_kind_t {
  failure_v,
  variable_v,
  function_v,
  operator_v,
  lparen_v,
  rparen_v,
  constant_v,
};

/// Common type for token specifications. All tokens have an identifier and a
/// kind. More specific data can be attached for children, such as precedence
/// for operators or arity for functions.
struct token_base_t {
  token_kind_t kind;
  std::string_view text;

  constexpr virtual ~token_base_t() = default;

  constexpr token_base_t(token_kind_t kind_, std::string_view text_)
      : kind(kind_), text(text_) {}

  /// Calls visitor with current token casted to its underlaying type.
  template <typename VisitorType> constexpr auto visit(VisitorType visitor);

  /// Calls visitor with current token casted to its underlaying const type.
  template <typename VisitorType>
  constexpr auto visit(VisitorType visitor) const;

  /// Returns true if a valid result is held by the object.
  constexpr operator bool() const { return kind != failure_v; }
};

/// Empty token for parsing failure management
struct failure_t : token_base_t {
  constexpr failure_t() : token_base_t(failure_v, {}) {}
};

/// Variable spec type
struct variable_t : token_base_t {
  constexpr variable_t(std::string_view identifier)
      : token_base_t(variable_v, identifier) {}
};

/// Function spec type
struct function_t : token_base_t {
  constexpr function_t(std::string_view identifier)
      : token_base_t(function_v, identifier) {}
};

/// Operator associativity
enum operator_associativity_t { left_v, right_v };

/// Operator spec type
struct operator_t : token_base_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr operator_t(std::string_view identifier,
                       operator_associativity_t associativity_,
                       unsigned precedence_)
      : token_base_t(operator_v, identifier), associativity(associativity_),
        precedence(precedence_) {}
};

/// Left parenthesis spec type
struct lparen_t : token_base_t {
  constexpr lparen_t(std::string_view identifier)
      : token_base_t(lparen_v, identifier) {}
};

/// Right parenthesis spec type
struct rparen_t : token_base_t {
  constexpr rparen_t(std::string_view identifier)
      : token_base_t(rparen_v, identifier) {}
};

/// Constant (unsigned integer)
struct constant_t : token_base_t {
  unsigned value;
  constexpr constant_t(unsigned value_, std::string_view number)
      : token_base_t(constant_v, number), value(value_) {}
};

template <typename VisitorType>
constexpr auto token_base_t::visit(VisitorType visitor) {
  switch (kind) {
  case failure_v:
    return visitor(static_cast<failure_t &>(*this));
  case variable_v:
    return visitor(static_cast<variable_t &>(*this));
  case function_v:
    return visitor(static_cast<function_t &>(*this));
  case operator_v:
    return visitor(static_cast<operator_t &>(*this));
  case lparen_v:
    return visitor(static_cast<lparen_t &>(*this));
  case rparen_v:
    return visitor(static_cast<rparen_t &>(*this));
  case constant_v:
    return visitor(static_cast<constant_t &>(*this));
  }
}

template <typename VisitorType>
constexpr auto token_base_t::visit(VisitorType visitor) const {
  switch (kind) {
  case failure_v:
    return visitor(static_cast<failure_t const &>(*this));
  case variable_v:
    return visitor(static_cast<variable_t const &>(*this));
  case function_v:
    return visitor(static_cast<function_t const &>(*this));
  case operator_v:
    return visitor(static_cast<operator_t const &>(*this));
  case lparen_v:
    return visitor(static_cast<lparen_t const &>(*this));
  case rparen_v:
    return visitor(static_cast<rparen_t const &>(*this));
  case constant_v:
    return visitor(static_cast<constant_t const &>(*this));
  }
}

/// Grammar specification that defines a formula to recognizable with the
/// shunting-yard algorithm.
struct token_specification_t {
  std::vector<variable_t> variables;
  std::vector<function_t> functions;
  std::vector<operator_t> operators;
  std::vector<lparen_t> lparens;
  std::vector<rparen_t> rparens;
};

/// Represents the parsing result of parse_formula.
struct rpn_result_t {
  /// List of tokens (RPN notation)
  std::vector<token_base_t const *> output_queue;

  /// Polymorphic buffer for tokens that aren't already held in the
  /// grammar_spec_t object (constants)
  std::vector<unique_ptr_selector::unique_ptr<token_base_t>> token_memory;
};

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

  /// Conversion function
  constexpr literal_failure_t(failure_t const &non_literal)
      : literal_failure_t() {}
};

/// Literal version for variable spec type
struct literal_variable_t : literal_token_base_t {
  constexpr literal_variable_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_variable_t(variable_t const &non_literal)
      : literal_variable_t(non_literal.text) {}
};

/// Literal version for function spec type
struct literal_function_t : literal_token_base_t {
  constexpr literal_function_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
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

  /// Conversion function
  constexpr literal_operator_t(operator_t const &non_literal)
      : literal_operator_t(non_literal.text, non_literal.associativity,
                           non_literal.precedence) {}
};

/// Literal version for left parenthesis spec type
struct literal_lparen_t : literal_token_base_t {
  constexpr literal_lparen_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_lparen_t(lparen_t const &non_literal)
      : literal_lparen_t(non_literal.text) {}
};

/// Literal version for right parenthesis spec type
struct literal_rparen_t : literal_token_base_t {
  constexpr literal_rparen_t(std::string_view identifier)
      : literal_token_base_t{identifier} {}

  /// Conversion function
  constexpr literal_rparen_t(rparen_t const &non_literal)
      : literal_rparen_t(non_literal.text) {}
};

/// Constant (unsigned integer)
struct literal_constant_t : literal_token_base_t {
  unsigned value;
  constexpr literal_constant_t(unsigned value_, std::string_view number)
      : literal_token_base_t{number}, value(value_) {}

  /// Conversion function
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

/// Helper kind getter for literal_failure_t.
constexpr token_kind_t get_kind(literal_failure_t const &) { return failure_v; }
/// Helper kind getter for literal_variable_t.
constexpr token_kind_t get_kind(literal_variable_t const &) {
  return variable_v;
}
/// Helper kind getter for literal_function_t.
constexpr token_kind_t get_kind(literal_function_t const &) {
  return function_v;
}
/// Helper kind getter for literal_operator_t.
constexpr token_kind_t get_kind(literal_operator_t const &) {
  return operator_v;
}
/// Helper kind getter for literal_lparen_t.
constexpr token_kind_t get_kind(literal_lparen_t const &) { return lparen_v; }
/// Helper kind getter for literal_rparen_t.
constexpr token_kind_t get_kind(literal_rparen_t const &) { return rparen_v; }
/// Helper kind getter for literal_constant_t.
constexpr token_kind_t get_kind(literal_constant_t const &) {
  return constant_v;
}

} // namespace shunting_yard
