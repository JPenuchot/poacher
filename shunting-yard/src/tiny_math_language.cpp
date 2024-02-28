#include <algorithm>
#include <blaze/math/dense/DynamicVector.h>
#include <utility>

#include <fmt/core.h>

#include <blaze/Blaze.h>

#include <tiny_math_language.hpp>

int main() {
  namespace tml = tiny_math_language;
  static constexpr auto formula =
      "sin((x + 3) / 3 * y ^ 2)";

  // Runtime parsing prints parsing steps
  // for debugging
  tml::parse(formula);

  // Input vectors
  constexpr std::size_t vec_size = 16;
  blaze::DynamicVector<float> vector_x(vec_size, 1.);
  blaze::DynamicVector<float> vector_y(vec_size, 12.);

  // Generating code from the formula
  auto function = tml::codegen<formula>();

  // Running the generated code
  blaze::DynamicVector<float> result =
      function(vector_x, vector_y);

  // Printing the result
  for (float const &element : result) {
    fmt::print("{} ", element);
  }
  fmt::print("\n");
}
