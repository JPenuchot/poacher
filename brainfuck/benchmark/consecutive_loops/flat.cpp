#include <boost/preprocessor/repetition/repeat.hpp>

#define REPEAT_STRING(z, n, str) str

constexpr char const *program_string =
    BOOST_PP_REPEAT(BENCHMARK_SIZE, REPEAT_STRING, "[+]+");

int main () {}
