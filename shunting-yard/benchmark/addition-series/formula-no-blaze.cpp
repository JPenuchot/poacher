#include <boost/preprocessor/repetition/repeat.hpp>

#include <tiny_math_language.hpp>

// Generating a series of additions
#define REPEAT_STRING(z, n, str) str
static constexpr const char *program_string =
    BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING,
                    "x + y + ") "x";

/// Benchmark entry point.
template <typename T = void> inline auto bench_me() {
  int a = 32;
  int b = 32;

  int res =
      tiny_math_language::codegen<program_string>()(
          a, b);

  return res;
}

void foo() { bench_me(); }
