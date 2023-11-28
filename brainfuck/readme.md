# Brainfuck

This is a Brainfuck compiler. In a compiler.

It currently has two backends:
- An expression-template based backend as a baseline,
- An `std::array` based backend, meant to explore value-based metaprogramming:
  how it performs, what tools are made available by using regular programming
  tools for metaprogramming (debuggers, test suites, etc)
