#pragma once

#include <brainfog/backends/pass_by_generator.hpp>
#include <brainfog/parsers/naive.hpp>
#include <brainfog/program.hpp>

template <auto const &ProgramString> inline void run_program() {
  brainfog::program_state_t s;

  brainfog::pass_by_generator::codegen<[]() -> brainfog::ast_node_ptr_t {
    return brainfog::naive_parser::parse_ast(ProgramString);
  }>()(s);
}
