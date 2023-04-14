#include <algorithm>
#include <utility>

#include <fmt/core.h>

#include <blaze/Blaze.h>

#include <rubbish_algebra.hpp>

int main() {
  static constexpr auto formula = "sin((x + 3) / 3 * y ^ 2)";

  // Runtime parsing prints parsing steps
  simple_algebra::parse(formula);

  // Input vectors
  constexpr std::size_t vec_size = 16;
  blaze::DynamicVector<float> vector_x(vec_size, 1.);
  blaze::DynamicVector<float> vector_y(vec_size, 12.);

  auto function = simple_algebra::codegen<formula>();

  // Processing the formula into an expression template, and printing it.
  for (float const e : function(vector_x, vector_y)) {
    fmt::print("{} ", e);
  }
  fmt::print("\n");
}
