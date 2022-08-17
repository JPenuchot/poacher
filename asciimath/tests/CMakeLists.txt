file(GLOB_RECURSE SOURCES asciimath/*.cpp)

find_package(Catch2 REQUIRED)

add_executable(tests ${SOURCES} main.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2)
target_include_directories(tests PRIVATE ../include)
