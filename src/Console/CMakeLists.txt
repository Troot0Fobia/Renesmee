add_library(console console.cpp console.hpp)

target_link_libraries(console PRIVATE cxxopts)
