#include <string_view>

#include <cest/memory.hpp>
#include <vector>

// https://en.wikipedia.org/wiki/Shunting_yard_algorithm

struct token_spec_t {
  constexpr virtual std::string_view get_token() const = 0;
};

struct function_spec_t : token_spec_t {
  std::string_view identifier;
};

struct operator_spec_t : token_spec_t {
  std::string_view identifier;
};

struct grammar_spec_t : token_spec_t {
  std::vector<std::string_view> variables;
  std::vector<function_spec_t> functions;
  std::vector<operator_spec_t> operators;

  int left_paren;
  int right_paren;
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
  /* After the while loop, pop the remaining items from the operator stack into the output queue. */
  while (false /* there are tokens on the operator stack */) {
    /* If the operator token on the top of the stack is a parenthesis, then
     * there are mismatched parentheses. */

    // {assert the operator on top of the stack is not a (left) parenthesis}

    // pop the operator from the operator stack onto the output queue
  }

  return {};
}

int main() {}
