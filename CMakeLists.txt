cmake_minimum_required(VERSION 3.20)

project(
    RenesmeeCLI
    VERSION 0.0.1
    LANGUAGES CXX
)

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (e.g. build) and run CMake from threre.")
endif()

add_subdirectory(libs/cxxopts)
add_subdirectory(libs/toml11)
add_subdirectory(src)
