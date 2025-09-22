cmake_minimum_required(VERSION 3.20)

add_library(adapters STATIC
    models/config/config.cpp
    models/console/console.cpp
    models/plugin/plugin.cpp
)
target_compile_features(adapters PUBLIC cxx_std_20)

target_include_directories(adapters PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/models/config
    ${CMAKE_CURRENT_SOURCE_DIR}/models/console
    ${CMAKE_CURRENT_SOURCE_DIR}/models/plugin

    ${PROJECT_SOURCE_DIR}/includes

    ${DOMAIN_INCLUDES}
    ${UTILS_INCLUDES}
    ${LOADER_INCLUDES}
)
target_link_libraries(adapters PUBLIC
    toml11::toml11
    cxxopts
    utils
    domain
    lib_loader
)
