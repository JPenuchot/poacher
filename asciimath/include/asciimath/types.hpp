#pragma once

#include <cest/memory.hpp>

#include <string>
#include <string_view>
#include <variant>

// AsciiMath grammar:

// identifier = [A-z]
// symbol = /* any string in the symbol table */
// number = '-'? [0-9]+ ( '.' [0-9]+ )?
// constant = number | symbol | identifier
// text = '"' [^"]* '"'
// binary_op = 'frac' | 'root' | 'stackrel'
// binary_expr = binary_op simp simp
// unary_op = 'sqrt' | 'text'
// unary_expr = unary_op simp
// rparen = ')' | ']' | '}' | ':)' | ':}'
// lparen = '(' | '[' | '{' | '(:' | '{:'
// paren_expr = lparen expr rparen
// sub =  '_' simp super?
// super = '^' simp
// fraction = '/' simp
// simp = constant | paren_expr | unary_expr | binary_expr | text
// expr = ( simp ( fraction | sub | super ) )+

// Notes:
// - std::string_view are being used to represent tokens, it is therefore
// assumed that AST elements won't outlive the parser
// - simp_ptr_t is being used to recurse over simp_t

namespace asciimath {

// Simp forward declaration

struct simp_t;
using simp_ptr_t = cest::unique_ptr<simp_t>;
template <typename... Args> constexpr simp_ptr_t make_simp_ptr(Args &&...args) {
  return cest::make_unique<simp_t>(std::forward<Args>(args)...);
}

// Expr forward declaration

struct expr_t;
using expr_ptr_t = cest::unique_ptr<expr_t>;
template <typename... Args> constexpr expr_ptr_t make_expr_ptr(Args &&...args) {
  return cest::make_unique<expr_t>(std::forward<Args>(args)...);
}

/// identifier := [A-z]
struct identifier_t {
  std::string_view id;
};

/// symbol := /* any string in the symbol table */
struct symbol_t {
  std::string_view sym;
};

// TODO: Symbol table

/// number := '-'? [0-9]+ ( '.' [0-9]+ )?
struct number_t {
  int val;
};

/// constant := number | symbol | identifier
struct constant_t {
  std::variant<identifier_t, symbol_t, number_t> value;
};

/// text := '"' [^"]* '"'
using text_t = std::string_view; // TODO

/// binary_op := 'frac' | 'root' | 'stackrel'
using binary_op_t = std::string_view; // TODO

/// binary_expr := binary_op simp simp
struct binary_expr_t {
  binary_op_t op;

  simp_ptr_t simp_a;
  simp_ptr_t simp_b;
};

/// unary_op := 'sqrt' | 'text'
using unary_op_t = std::string_view; // TODO

/// unary_expr := unary_op simp
struct unary_expr_t {
  unary_op_t op;
  simp_ptr_t simp;
};

/// rparen := ')' | ']' | '}' | ':)' | ':}'
using rparen_t = std::string_view; // TODO

/// lparen := '(' | '[' | '{' | '(:' | '{:'
using lparen_t = std::string_view; // TODO

/// paren_expr := lparen expr rparen
struct paren_expr_t {
  lparen_t lparen;
  expr_ptr_t expr;
  rparen_t rparen;
};

/// sub :=  '_' simp super?
using sub_t = std::string_view; // TODO

/// super := '^' simp
using super_t = std::string_view; // TODO

/// fraction := '/' simp
using fraction_t = std::string_view; // TODO

/// simp := constant | paren_expr | unary_expr | binary_expr | text
struct simp_t {
  std::variant<constant_t, paren_expr_t, unary_expr_t, binary_expr_t, text_t>
      val;
};

/// expr := ( simp ( fraction | sub | super ) )+
struct expr_t {
    std::string_view remove_me;
}; // TODO

} // namespace asciimath
