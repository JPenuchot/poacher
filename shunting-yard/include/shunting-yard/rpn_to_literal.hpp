#pragma once

#include <array>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <variant>

#include <kumi/tuple.hpp>

#include <shunting-yard/parse_to_rpn.hpp>
#include <shunting-yard/types.hpp>

namespace shunting_yard {

/// Tuple implementation choice (kumi or std)
namespace tuple_implementation = kumi;
// namespace tuple_implementation = std;

/// Flattens an rpn result to a vector of literal_token_t values.
constexpr std::vector<literal_token_t>
flatten_rpn_result(rpn_result_t const &rpn_input,
                   token_specification_t const &spec) {
  std::vector<literal_token_t> result;
  for (token_base_t const *token_ptr : rpn_input.output_queue) {
    token_ptr->visit(
        [&](auto const &typed_token) { result.push_back({typed_token}); });
  }
  return result;
}

/// Asuming Fun is a constexpr function that returns a std::vector value,
/// eval_as_array will store its contents into an std::array.
template <auto Fun> constexpr auto eval_as_array() {
  constexpr std::size_t Size = Fun().size();
  std::array<typename decltype(Fun())::value_type, Size> res;
  std::ranges::copy(Fun(), res.begin());
  return res;
}

/// Converts an array of variants into a tuple of
/// its value as their underlaying types.
template <auto const &ArrayOfVariants>
constexpr auto array_of_variants_to_tuple() {
  // Storing the size of the array in a constexpr variable.
  constexpr std::size_t Size = ArrayOfVariants.size();

  // Static for-loop on array indexes
  return []<std::size_t... ArrayIndexPack>(
      std::index_sequence<ArrayIndexPack...>) {
    // Making a tuple of the array elements
    return tuple_implementation::make_tuple((
        // Unrolling the fold expression into a lambda that extracts the variant
        // elements into values of their actual types
        []<std::size_t UnpackedArrayIndex>(
            std::integral_constant<std::size_t, UnpackedArrayIndex>) {
          // Extracting type index into a constexpr variable
          constexpr std::size_t TypeIndex =
              ArrayOfVariants[UnpackedArrayIndex].index();
          // Getting the underlaying value
          return std::get<TypeIndex>(ArrayOfVariants[UnpackedArrayIndex]);
        }(std::integral_constant<std::size_t, ArrayIndexPack>{}),
        ...));
  }
  (std::make_index_sequence<Size>{});
}

/// For each token in RPNStackAsTuple, consume_tokens will call the
template <auto const &RPNStackAsTuple,
          template <auto const &, std::size_t> typename FrontTokenConsumerType,
          std::size_t RPNStackIndex = 0>
constexpr auto
consume_tokens(auto stack_as_tuple = tuple_implementation::make_tuple()) {
  // If no token is left to handle, return the value stack
  if constexpr (constexpr std::size_t RPNStackSize = std::tuple_size_v<
                    std::remove_cvref_t<decltype(RPNStackAsTuple)>>;
                RPNStackIndex >= RPNStackSize) {
    return stack_as_tuple;
  }
  // Otherwise, apply stack for given token and recurse on next token
  else if constexpr (RPNStackIndex < RPNStackSize) {
    // Apply current stack and pass front token as a template parameter
    return consume_tokens<RPNStackAsTuple, FrontTokenConsumerType,
                          RPNStackIndex + 1>(tuple_implementation::apply(
        FrontTokenConsumerType<RPNStackAsTuple, RPNStackIndex>::function,
        stack_as_tuple));
  }
}

} // namespace shunting_yard
