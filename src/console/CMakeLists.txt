cmake_minimum_required(VERSION 3.20)

add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE ${CMAKE_SOURCE_DIR}/libs/cxxopts)

add_library(console console.cpp console.hpp)

target_link_libraries(console PRIVATE cxxopts)
