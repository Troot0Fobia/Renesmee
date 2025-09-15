cmake_minimum_required(VERSION 3.20)

project(RenesmeeCLI)

add_subdirectory(libs/cxxopts)
add_subdirectory(libs/toml11)

add_executable(RenesmeeCLI src/main.cpp)

target_link_libraries(RenesmeeCLI PRIVATE cxxopts toml11::toml11)
