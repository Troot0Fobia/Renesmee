cmake_minimum_required(VERSION 3.20)

project(RenesmeeCLI)

add_subdirectory(libs/cxxopts)

add_executable(RenesmeeCLI src/main.cpp)

target_link_libraries(RenesmeeCLI PRIVATE cxxopts)
