#pragma once

#include <brainfog/backends/expression_template.hpp>
#include <brainfog/parsers/naive.hpp>
#include <brainfog/program.hpp>

template <auto const &ProgramString> auto run_program() {
  brainfog::program_state_t s;

  brainfog::expression_template::run(
      brainfog::expression_template::to_et(
          []() { return brainfog::naive_parser::parse_ast(ProgramString); }),
      s);
}
