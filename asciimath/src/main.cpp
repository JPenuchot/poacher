#include <iostream>

#include <asciimath/asciimath.hpp>

int main(int argc, char *argv[]) {
//   for (asciimath::symbols::symbol_def_t const &def :
//        asciimath::symbols::symbols_by_kind<asciimath::symbols::leftbracket_v>) {
//     std::cout << def.input << "\n";
//   }

  for (asciimath::symbols::symbol_def_t const &def :
       asciimath::symbols::symbols_by_kind<asciimath::symbols::const_v>) {
    std::cout << def.input << "\n";
  }

//   for (asciimath::symbols::symbol_def_t const &def :
//        asciimath::symbols::symbols_by_kind<asciimath::symbols::unary_v>) {
//     std::cout << def.input << "\n";
//   }
}
