#include <cest/iostream.hpp>
#include <cest/vector.hpp>

#include <brainfuck/brainfuck.hpp>

#include <type_traits>
#include <utility>

template<typename F, typename V>
constexpr auto
eval_to_array(F f, V visitor)
{
  constexpr auto sz = f().size();
  std::array<typename std::decay_t<decltype(f())>::value_type, sz> res;
  for (std::size_t i = 0; i < sz; i++)
    res[i] = visitor(f()[i]);
  return res;
}

int
main()
{
  auto constexpr arr = eval_to_array(
    []() constexpr {
      cest::vector<int> v;
      v.push_back(20);
      v.push_back(32);
      v.push_back(52);
      return v;
    },
    []<typename E>(E&& e) { return std::forward<E>(e); });

  static_assert(arr[2] == 52);

  for (auto v : arr)
    cest::cout << v << '\n';

  return 0;
}
