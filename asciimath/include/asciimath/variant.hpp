#include <utility>

#include <cest/memory.hpp>

namespace asciimath {

namespace detail {

/// Base class for polymorphic storage
struct storage_base_t {};

/// Daughter class for polymorphic storage
template <typename ValueType> struct storage_proxy_t : storage_base_t {
  ValueType value;
};

template<typename ValueType, std::size_t Id>
struct variant_for_type_t {

};

} // namespace detail

template <typename... ValueTypes> class variant_t {
  cest::unique_ptr<detail::storage_base_t> storage;

public:

};

} // namespace asciimath
