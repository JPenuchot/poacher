#pragma once

#include <ostream>

#include <grapher/core.hpp>

namespace grapher {

std::ostream &to_csv(std::ostream &o, categories_t const &cats);

} // namespace grapher
