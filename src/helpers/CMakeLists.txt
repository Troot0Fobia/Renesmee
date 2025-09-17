cmake_minimum_required(VERSION 3.20)

set(HELPERS_LIST
    parsers/config/config.cpp
    parsers/console/console.cpp
    # parsers/toml/toml_from.cpp
)

add_library(helpers ${HELPERS_LIST})

target_compile_features(helpers PRIVATE cxx_std_20)

target_include_directories(helpers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/console
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/config
    # ${CMAKE_CURRENT_SOURCE_DIR}/parsers/toml

    ${PROJECT_SOURCE_DIR}/src/models
)

target_link_libraries(helpers PUBLIC cxxopts toml11::toml11)
