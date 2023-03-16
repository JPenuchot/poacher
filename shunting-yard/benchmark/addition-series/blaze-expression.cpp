#include <boost/preprocessor/repetition/repeat.hpp>

#include <blaze/Blaze.h>

// Generating a series of additions
#define REPEAT_STRING(z, n, str) str

/// Benchmark entry point.
template <typename T = void> inline auto bench_me() {
  blaze::DynamicVector<unsigned int> vector_a(32, 12);
  blaze::DynamicVector<unsigned int> vector_b(32, 12);

  blaze::DynamicVector<unsigned int> res =
      BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING, vector_a + vector_b +)
          vector_a;

  return res;
}
