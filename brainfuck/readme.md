# Brainfuck

This is a Brainfuck compiler. In a compiler.

It was written to be as casual as a Brainfuck compiler would be, except it use
constexpr programming to then generate an expression template representation of
the program, that can be translated into a regular program, effectively
compiling it.

Poacher currently has 3 backends for the code generation part:
- Expression template, working but with poor performance
- NTTP (**not working yet**, waiting for
[propconst/P1974](https://wg21.link/P1974))
- Meta/fragment, WIP

```cpp
constexpr std::vector<int> foo();

constexpr std::size_t n = foo().size();
constexpr std::array<int, n> a = [] {
    std::array<int, n> to_leak;
    std::vector v = foo();
    std::copy_n(v.begin(), n, to_leak.begin());
    return to_leak;
}();
```
