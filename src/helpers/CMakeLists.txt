cmake_minimum_required(VERSION 3.20)

add_subdirectory(loader)

set(HELPERS_LIST
    parsers/console/console.cpp
    parsers/config/config.cpp
    parsers/toml/toml_from.cpp
    filesystem/filesystem_resolver.cpp
    parsers/inputs/inputs.cpp
)

add_library(helpers ${HELPERS_LIST})

target_compile_features(helpers PRIVATE cxx_std_20)

target_include_directories(helpers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/console
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/config
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/toml
    ${CMAKE_CURRENT_SOURCE_DIR}/parsers/inputs
    ${CMAKE_CURRENT_SOURCE_DIR}/filesystem

    ${PROJECT_SOURCE_DIR}/src/models
    ${PROJECT_SOURCE_DIR}/src/models/inputs
)

target_link_libraries(helpers PUBLIC cxxopts toml11::toml11 lib_loader)
