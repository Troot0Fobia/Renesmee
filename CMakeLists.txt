cmake_minimum_required(VERSION 3.20)

project(RenesmeeCLI)

add_subdirectory(libs/cxxopts)
add_subdirectory(libs/toml11)

add_executable(RenesmeeCLI src/main.cpp)
target_compile_features(RenesmeeCLI PUBLIC cxx_std_20)

target_link_libraries(RenesmeeCLI PRIVATE cxxopts toml11::toml11)
target_include_directories(RenesmeeCLI PRIVATE src/modules)
