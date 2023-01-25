#pragma once

#include <string_view>

namespace shunting_yard {

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
struct token_t {
  token_kind_t kind;
  std::string_view text;

  constexpr virtual ~token_t() = default;

  constexpr token_t(token_kind_t kind_, std::string_view text_)
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
struct failure_t : token_t {
  constexpr failure_t() : token_t(failure_v, {}) {}
};

/// Variable spec type
struct variable_t : token_t {
  constexpr variable_t(std::string_view identifier)
      : token_t(variable_v, identifier) {}
};

/// Function spec type
struct function_t : token_t {
  constexpr function_t(std::string_view identifier)
      : token_t(function_v, identifier) {}
};

/// Operator associativity
enum operator_associativity_t { left_v, right_v };

/// Operator spec type
struct operator_t : token_t {
  operator_associativity_t associativity;
  unsigned precedence;

  constexpr operator_t(std::string_view identifier,
                       operator_associativity_t associativity_,
                       unsigned precedence_)
      : token_t(operator_v, identifier), associativity(associativity_),
        precedence(precedence_) {}
};

/// Left parenthesis spec type
struct lparen_t : token_t {
  constexpr lparen_t(std::string_view identifier)
      : token_t(lparen_v, identifier) {}
};

/// Right parenthesis spec type
struct rparen_t : token_t {
  constexpr rparen_t(std::string_view identifier)
      : token_t(rparen_v, identifier) {}
};

/// Constant (unsigned integer)
struct constant_t : token_t {
  unsigned value;
  constexpr constant_t(unsigned value_, std::string_view number)
      : token_t(constant_v, number), value(value_) {}
};

template <typename VisitorType>
constexpr auto token_t::visit(VisitorType visitor) {
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
constexpr auto token_t::visit(VisitorType visitor) const {
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

} // namespace shunting_yard
