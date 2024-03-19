#pragma once

#include <brainfuck/backends/expression_template.hpp>
#include <brainfuck/parser.hpp>
#include <brainfuck/program.hpp>

template <auto const &ProgramString>
inline void run_program() {
  brainfuck::program_state_t s;

  auto fun = brainfuck::expression_template::codegen(
      brainfuck::expression_template::to_et([]() {
        return brainfuck::parser::parse_ast(
            ProgramString);
      }));

  fun(s);
}
