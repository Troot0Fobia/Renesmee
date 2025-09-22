cmake_minimum_required(VERSION 3.20)

add_library(domain STATIC
    DTOs/console_args.hpp
    value_objects/address.hpp
    value_objects/plugin_info.hpp
    value_objects/proxy.hpp
    value_objects/result.hpp
    value_objects/version.cpp
    value_objects/version.hpp
)

target_compile_features(domain PRIVATE cxx_std_20)

target_include_directories(domain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/DTOs
    ${CMAKE_CURRENT_SOURCE_DIR}/value_objects
)
