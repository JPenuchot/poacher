# Brainfuck

This is a Brainfuck compiler. In a compiler.

It was written to be as casual as a Brainfuck compiler would be, except it use
constexpr programming to then generate an expression template representation of
the program, that can be translated into a regular program, effectively
compiling it.

- [`brainfuck/ast.hpp`](include/brainfuck/ast.hpp): Definition of the
  Brainfuck AST
- [`brainfuck/ir.hpp`](include/brainfuck/ir.hpp): Definition of the template
  intermediate representation
- [`brainfuck/parser.hpp`](include/brainfuck/parser.hpp): Brainfuck AST parser
- [`brainfuck/ast_to_ir.hpp`](include/brainfuck/ast_to_ir.hpp): Intermediate
  representation from AST
- [`brainfuck/program.hpp`](include/brainfuck/program.hpp): Code generation
  from intermediate representation
