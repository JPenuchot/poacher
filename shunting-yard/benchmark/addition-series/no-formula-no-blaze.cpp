#include <boost/preprocessor/repetition/repeat.hpp>

// Generating a series of additions
#define REPEAT_STRING(z, n, str) str

/// Benchmark entry point.
template <typename T = void> inline auto bench_me() {
  int a = 32;
  int b = 32;

  int res = BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING, a + b +) a;

  return res;
}

void foo() { bench_me(); }
