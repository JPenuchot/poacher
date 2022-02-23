# Brainfuck

This is a Brainfuck compiler. In a compiler.

It was written to be as casual as a Brainfuck compiler would be, except it use
constexpr programming to then generate an expression template representation of
the program, that can be translated into a regular program, effectively
compiling it.

Poacher currently has 2 IRs:
- Expression template, working but with poor performance
- Flattened AST IR in a fixed array, a bit of work is still needed
  for the actual code generation part but nothing too terrible
