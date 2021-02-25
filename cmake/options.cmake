# CMAKE OPTIONS DECLARATIONS

option(POACHER_TIDY
"Run clang-tidy"
OFF)

option(POACHER_METACLASS
"Specifies whether metaclass code should be compiled \
(requires a fragment-compatible compiler)"
OFF)

# COMPILE FLAGS

if(POACHER_TIDY)
  message("[WARNING] avoid running benchmarks with POACHER_TIDY turned on")
  set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
endif()

if(POACHER_METACLASS)
  message("[INFO] metaclass features enabled")
  add_compile_options("-freflection" "-DPOACHER_METACLASS=1")
endif()
