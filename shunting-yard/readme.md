# Constexpr Shunting Yard parser

This subproject is a `constexpr` compatible implementation of
[Dijkstra's Shunting Yard algorithm](
https://en.wikipedia.org/wiki/Shunting_yard_algorithm), which means the produced
result can be used to produce a constexpr value.

It can recognize mathematical formulas and turn them into Reverse Polish
Notation (RPN). Tokens such as operators, functions, variables, or parenthesis
are user-defined.

Example:

Starting formula: `sin ( max ( 2, 3 ) / 3 * pi ^ 2 )`
Result: `2 3 max 3 / pi 2 ^ * sin`



## Dependencies

- [fmt](https://fmt.dev/)
- [Kumi](https://jfalcou.github.io/kumi/)
- [Blaze](https://bitbucket.org/blaze-lib/blaze)
