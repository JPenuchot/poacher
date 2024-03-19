#pragma once

#include <brainfuck/backends/pass_by_generator.hpp>
#include <brainfuck/parser.hpp>
#include <brainfuck/program.hpp>

template <auto const &ProgramString>
inline void run_program() {
  brainfuck::program_state_t s;

  auto fun = brainfuck::pass_by_generator::codegen<
      []() -> brainfuck::ast_node_ptr_t {
        return brainfuck::parser::parse_ast(
            ProgramString);
      }>();

  fun(s);
}
