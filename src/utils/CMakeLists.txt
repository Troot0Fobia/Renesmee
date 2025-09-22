cmake_minimum_required(VERSION 3.20)

add_library(utils STATIC filesystem_utils.cpp)
target_compile_features(utils PRIVATE cxx_std_20)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
