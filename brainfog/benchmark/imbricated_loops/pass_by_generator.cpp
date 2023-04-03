#include <boost/preprocessor/repetition/repeat.hpp>

#include <benchmark/pass_by_generator.hpp>

#define REPEAT_STRING(z, n, str) str

constexpr char const *program_string =
    BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING, "[")
        BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING, "+]+");

template <typename T = void> inline void bench_me() { run_program<program_string>(); }
void foo() { bench_me(); }
