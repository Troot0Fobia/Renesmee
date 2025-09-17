cmake_minimum_required(VERSION 3.20)

project(RenesmeeCLI)

add_subdirectory(libs/cxxopts)
add_subdirectory(libs/toml11)
add_subdirectory(src/helpers)

add_executable(RenesmeeCLI src/main.cpp)
target_compile_features(RenesmeeCLI PUBLIC cxx_std_20)

target_link_libraries(RenesmeeCLI PRIVATE helpers cxxopts)
target_include_directories(RenesmeeCLI PRIVATE src/models)
