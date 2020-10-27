#include <cest/iostream.hpp>
#include <cest/vector.hpp>

#include <poacher/brainfuck.hpp>

#include <type_traits>
#include <utility>

template<typename F>
constexpr auto
eval_to_array(F f)
{
  constexpr auto sz = f().size();
  std::array<typename std::decay_t<decltype(f())>::value_type, sz> res;
  for (std::size_t i = 0; i < sz; i++)
    res[i] = f()[i];
  return res;
}

int
main()
{
  using namespace std;

  auto constexpr arr = eval_to_array([]() constexpr {
    cest::vector<int> v;
    v.push_back(20);
    v.push_back(32);
    v.push_back(52);
    return v;
  });

  static_assert(arr[2] == 52);

  for (auto v : arr)
    cest::cout << v << '\n';

  return 0;
}
