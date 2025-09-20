cmake_minimum_required(VERSION 3.20)

set(SOURCE_LIST
    plugin/plugin.cpp
)

add_library(models ${SOURCE_LIST})
target_compile_features(models PUBLIC cxx_std_20)

target_include_directories(models PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
    ${PROJECT_SOURCE_DIR}/src/helpers/loader/core
    ${CMAKE_CURRENT_SOURCE_DIR}/inputs
    ${PROJECT_SOURCE_DIR}/src/helpers/filesystem
)
