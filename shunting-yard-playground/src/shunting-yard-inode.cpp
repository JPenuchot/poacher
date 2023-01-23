#include <string_view>

#include <cest/memory.hpp>
#include <vector>

// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

/// Token kind for grammar spec
enum token_kind_t {
  variable_v,
  function_v,
  operator_v,
  lparen_v,
  rparen_v,
};

struct variable_spec_t;
struct function_spec_t;
struct operator_spec_t;
struct lparen_spec_t;
struct rparen_spec_t;

/// Common type for token specifications.
struct token_spec_t {
  token_kind_t kind;
  std::string_view identifier;
  constexpr token_spec_t(token_kind_t _kind, std::string_view identifier_)
      : kind(_kind), identifier(identifier_) {}

  constexpr std::string_view get_identifier() const { return identifier; }
  constexpr token_kind_t get_kind() const { return kind; }

  /// Calls visitor with current token casted to its underlaying type.
  template <typename Visitor> constexpr auto visit(Visitor visitor) {
    switch (kind) {
    case variable_v:
      return visitor(*(variable_spec_t *)(this));
    case function_v:
      return visitor(*(function_spec_t *)(this));
    case operator_v:
      return visitor(*(operator_spec_t *)(this));
    case lparen_v:
      return visitor(*(lparen_spec_t *)(this));
    case rparen_v:
      return visitor(*(rparen_spec_t *)(this));
    }
  }

  /// Calls visitor with current token casted to its underlaying const type.
  template <typename Visitor> constexpr auto visit(Visitor visitor) const {
    switch (kind) {
    case variable_v:
      return visitor(*(variable_spec_t const *)(this));
    case function_v:
      return visitor(*(function_spec_t const *)(this));
    case operator_v:
      return visitor(*(operator_spec_t const *)(this));
    case lparen_v:
      return visitor(*(lparen_spec_t const *)(this));
    case rparen_v:
      return visitor(*(rparen_spec_t const *)(this));
    }
  }
};

/// Variable spec type
struct variable_spec_t : token_spec_t {
  constexpr variable_spec_t(std::string_view identifier)
      : token_spec_t(variable_v, identifier) {}
};

/// Function spec type
struct function_spec_t : token_spec_t {
  constexpr function_spec_t(std::string_view identifier)
      : token_spec_t(function_v, identifier) {}
};

/// Operator spec type
struct operator_spec_t : token_spec_t {
  constexpr operator_spec_t(std::string_view identifier)
      : token_spec_t(operator_v, identifier) {}
};

/// Left parenthesis spec type
struct lparen_spec_t : token_spec_t {
  constexpr lparen_spec_t(std::string_view identifier)
      : token_spec_t(lparen_v, identifier) {}
};

struct rparen_spec_t : token_spec_t {
  constexpr rparen_spec_t(std::string_view identifier)
      : token_spec_t(rparen_v, identifier) {}
};

/// Grammar specification that defines a formula to recognizable with the
/// shunting-yard algorithm.
struct grammar_spec_t {
  std::vector<variable_spec_t> variables;
  std::vector<function_spec_t> functions;
  std::vector<operator_spec_t> operators;
  std::vector<lparen_spec_t> lparens;
  std::vector<rparen_spec_t> rparens;
};

std::vector<cest::unique_ptr<token_spec_t>> constexpr parse_formula(
    std::string_view formula, grammar_spec_t const &spec) {
  // The functions referred to in this algorithm are simple single argument
  // functions such as sine, inverse or factorial.

  // This implementation does not implement composite functions, functions
  // with a variable number of arguments, or unary operators.

  while (false /* there are tokens to be read */) {
    // read a token

    if (false /* token is a number */) {
      // put it into the output queue
    }

    if (false /* token is a function */) {
      // push it onto the operator stack
    }

    if (false /* token is an operator o1 */) {
      while (
          false
          // there is an operator o2 at the top of the operator stack which is
          // not a left parenthesis,
          // and (o2 has greater precedence than o1 or
          // (o1 and o2 have the same precedence and o1 is left-associative))
      ) {
        // pop o2 from the operator stack into the output queue
      }
      // push o1 onto the operator stack
    }

    if (false /* token is a left parenthesis (i.e. "(") */) {
      // push it onto the operator stack
    }

    if (false /* token is a right parenthesis (i.e. ")") */) {
      while (false
             // the operator at the top of the operator stack is not a left
             // parenthesis

      ) {
        // {assert the operator stack is not empty}
        /*  If the stack runs out without finding a left parenthesis,
            then there are mismatched parentheses. */
        // pop the operator from the operator stack into the output queue
      }
      // {assert there is a left parenthesis at the top of the operator stack}

      // pop the left parenthesis from the operator stack and discard it
      if(false/* there is a function token at the top of the operator stack */)
      {
        // pop the function from the operator stack into the output queue
      }
    }
  }
  /* After the while loop, pop the remaining items from the operator stack into
   * the output queue. */
  while (false /* there are tokens on the operator stack */) {
    /* If the operator token on the top of the stack is a parenthesis, then
     * there are mismatched parentheses. */

    // {assert the operator on top of the stack is not a (left) parenthesis}

    // pop the operator from the operator stack onto the output queue
  }

  return {};
}

int main() {}
