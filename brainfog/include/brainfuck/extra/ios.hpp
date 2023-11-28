#pragma once

#include <brainfuck/ast.hpp>
#include <cest/iostream.hpp>
#include <cest/string.hpp>

namespace brainfuck::extra {

inline std::ostream &
operator<<(std::ostream &, ast_node_ptr_t const &);

/// Converts a token into a string_view.
constexpr std::string_view to_string(token_t t) {
  switch (t) {
  case pointer_increase_v:
    return "++ptr;\n";
  case pointer_decrease_v:
    return "--ptr;\n";
  case pointee_increase_v:
    return "++*ptr;\n";
  case pointee_decrease_v:
    return "--*ptr;\n";
  case put_v:
    return "putchar(*ptr);\n";
  case get_v:
    return "*ptr=getchar();\n";
  case while_begin_v:
    return "while (*ptr) {\n";
  case while_end_v:
    return "}\n";
  case nop_v:
    return "nop\n";
  }
  return "";
}

/// Outputs a token node.
inline std::ostream &
operator<<(std::ostream &o, ast_token_t const &t) {
  return o << to_string(t.get_token());
}

/// Outputs an block node.
inline std::ostream &
operator<<(std::ostream &o, ast_block_t const &b) {
  for (ast_node_ptr_t const &n : b.get_content())
    o << n;
  return o;
}

/// Outputs a while node.
inline std::ostream &
operator<<(std::ostream &o, ast_while_t const &w) {
  return o << to_string(whb_v) << w.get_block()
           << to_string(whe_v);
}

/// Outputs a node pointer.
inline std::ostream &
operator<<(std::ostream &o, ast_node_ptr_t const &p) {
  if (isa<ast_token_t>(p))
    return o << *getas<ast_token_t>(p);
  if (isa<ast_block_t>(p))
    return o << *getas<ast_block_t>(p);
  if (isa<ast_while_t>(p))
    return o << *getas<ast_while_t>(p);

  return o;
}

} // namespace brainfuck::extra
