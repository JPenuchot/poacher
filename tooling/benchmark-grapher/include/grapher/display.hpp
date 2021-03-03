#pragma once

#include <filesystem>

#include <grapher/core.hpp>

namespace grapher {

void graph(categories_t const &, std::filesystem::path const &p);

} // namespace grapher
