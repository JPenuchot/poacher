#pragma once

#include <brainfuck/ir/expression_template.hpp>
#include <brainfuck/parsers/naive.hpp>
#include <brainfuck/program.hpp>

template <auto const &ProgramString> auto run_program() {
  brainfuck::program_state_t s;

  brainfuck::run(brainfuck::to_ir([]() {
                   return brainfuck::naive_parser::parse_ast(ProgramString);
                 }),
                 s);
}
