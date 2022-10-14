/// \file Driver

#pragma once

#include "brainfog/ir/expression_template.hpp"
#include "brainfog/ir/flat.hpp"
#include "brainfog/parsers/naive.hpp"
#include "brainfog/program.hpp"

namespace brainfog {

enum backend_t {
  expression_template_v,
  flat_v,
};

template <auto const &ProgramString> constexpr auto to_flat_ast() {
  namespace bf = brainfog;
  namespace bflat = bf::flat;

  // Calculating size
  constexpr size_t AstArraySize =
      bflat::flatten(bf::naive_parser::parse_ast(ProgramString)).size();

  // Building array from vector
  bflat::flat_ast_t ast_vec =
      bflat::flatten(bf::naive_parser::parse_ast(ProgramString));

  bflat::fixed_flat_ast_t<AstArraySize> arr;
  std::ranges::copy(ast_vec, arr.begin());

  return arr;
}

template <auto const &ProgramString>
void expression_template_run(program_state_t &s) {
  using IR =
      decltype(to_ir([]() { return naive_parser::parse_ast(ProgramString); }));

  run<IR>(s);
}

template <auto const &ProgramString> void flat_run(program_state_t &s) {

}

template <auto const &ProgramString, backend_t Backend>
void run_program(program_state_t &s) {
  if constexpr (Backend == expression_template_v) {
    expression_template_run<ProgramString>(s);
  } else if constexpr (Backend == flat_v) {
    flat_run<ProgramString>(s);
  }
}

} // namespace brainfog
