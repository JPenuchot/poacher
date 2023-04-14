#include <boost/preprocessor/repetition/repeat.hpp>

#include <blaze/Blaze.h>

#include <rubbish_algebra.hpp>

// Generating a series of additions
#define REPEAT_STRING(z, n, str) str
static constexpr const char *program_string =
    BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING, "x + y + ") "x";

/// Benchmark entry point.
template <typename T = void> inline auto bench_me() {
  blaze::DynamicVector<unsigned int> vector_a(32, 12);
  blaze::DynamicVector<unsigned int> vector_b(32, 12);

  blaze::DynamicVector<unsigned int> res =
      rubbish_algebra::codegen<program_string>()(vector_a, vector_b);

  return res;
}

void foo() { bench_me(); }
