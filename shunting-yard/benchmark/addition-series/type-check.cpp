#include <boost/preprocessor/repetition/repeat.hpp>

#include <blaze/Blaze.h>

#include <tiny_math_language.hpp>

// Generating a series of additions
#define REPEAT_STRING(z, n, str) str
static constexpr const char *program_string =
    BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING,
                    "x + y + ") "x";

template <typename T = void>
inline auto blaze_expression() {
  blaze::DynamicVector<unsigned int> vector_a(32, 12);
  blaze::DynamicVector<unsigned int> vector_b(32, 12);

  blaze::DynamicVector<unsigned int> res =
      tiny_math_language::codegen<program_string>()(
          vector_a, vector_b);

  return res;
}

template <typename T = void>
inline auto text_formula() {
  blaze::DynamicVector<unsigned int> vector_a(32, 12);
  blaze::DynamicVector<unsigned int> vector_b(32, 12);

  blaze::DynamicVector<unsigned int> res =
      BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING,
                      vector_a + vector_b +) vector_a;

  return res;
}

static_assert(
    std::is_same_v<decltype(blaze_expression()),
                   decltype(text_formula())>);

void foo() {
  blaze_expression();
  text_formula();
}
