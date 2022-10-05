#pragma once

#include <asciimath/symbols.hpp>

#include <cest/memory.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// AsciiMath grammar:

// identifier = [A-z]
// symbol = /* any string in the symbol table */
// number = '-'? [0-9]+ ( '.' [0-9]+ )?
// constant = number | symbol | identifier
// text = '"' [^"]* ' "'
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

// https://github.com/asciidoctor/asciimath/blob/master/lib/asciimath/parser.rb

// Notes:
// - std::string_view are being used to represent tokens, it is therefore
// assumed that AST elements won't outlive the parser

/// AsciiMath AST types
namespace asciimath::ast {

// Forward declarations for recursion using cest::unique_ptr

struct identifier_t;
struct symbol_t;
struct number_t;
struct constant_t;
struct text_t;
struct binary_op_t;
struct binary_expr_t;
struct unary_op_t;
struct unary_expr_t;
struct rparen_t;
struct lparen_t;
struct paren_expr_t;
struct sub_t;
struct super_t;
struct fraction_t;
struct simp_t;
struct expr_t;

/// ```identifier := [A-z]```
struct identifier_t {
  std::string_view id;
};

/// ```symbol := /* any string in the symbol table */```
struct symbol_t {
  std::string_view sym;
}; // TODO: Symbol table

/// ```number := '-'? [0-9]+ ( '.' [0-9]+ )?```
struct number_t {
  int val;
};

/// ```constant := number | symbol | identifier```
struct constant_t {
  std::variant<identifier_t, symbol_t, number_t> value;
};

/// ```text := '"' [^"]* '"'```
struct text_t {
  std::string_view val;
}; // TODO

/// ```binary_op := 'frac' | 'root' | 'stackrel'```
struct binary_op_t {
  std::size_t index;
};

/// ```binary_expr := binary_op simp simp```
struct binary_expr_t {
  binary_op_t op;

  cest::unique_ptr<simp_t> simp_a_ptr;
  cest::unique_ptr<simp_t> simp_b_ptr;
};

/// ```unary_op := 'sqrt' | 'text'```
struct unary_op_t {
  std::size_t index;
};

/// ```unary_expr := unary_op simp```
struct unary_expr_t {
  unary_op_t op;
  cest::unique_ptr<simp_t> simp_ptr;
};

/// ```rparen := ')' | ']' | '}' | ':)' | ':}'```
struct rparen_t {
  /// Index in symbols_by_kind<symbols::rightbracket_v>
  std::size_t index;
};

/// ```lparen := '(' | '[' | '{' | '(:' | '{:'```
struct lparen_t {
  /// Index in symbols_by_kind<symbols::leftbracket_v>
  std::size_t index;
};

/// ```paren_expr := lparen expr rparen```
struct paren_expr_t {
  lparen_t lparen;
  cest::unique_ptr<expr_t> expr_ptr;
  rparen_t rparen;
};

/// ```sub :=  '_' simp super?```
struct sub_t {
  cest::unique_ptr<simp_t> simp_ptr;
  std::optional<cest::unique_ptr<super_t>> super_ptr;
};

/// ```super := '^' simp```
struct super_t {
  cest::unique_ptr<simp_t> simp_ptr;
};

/// ```fraction := '/' simp```
struct fraction_t {
  cest::unique_ptr<simp_t> simp_ptr;
};

/// ```simp := constant | paren_expr | unary_expr | binary_expr | text```
struct simp_t {
  std::variant<constant_t, paren_expr_t, unary_expr_t, binary_expr_t, text_t>
      val;
};

/// ```( simp ( fraction | sub | super ) )```
struct expr_element_t {
  /// ```( fraction | sub | super )```
  using variant_t = std::variant<sub_t, fraction_t, super_t>;

  cest::unique_ptr<simp_t> simp_ptr;
  variant_t next;
};

/// ```expr := ( simp ( fraction | sub | super ) )+```
struct expr_t {
  std::vector<expr_element_t> content;
};

} // namespace asciimath::ast
