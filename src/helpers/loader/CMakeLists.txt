cmake_minimum_required(VERSION 3.20)

set(SOURCE_LIST)

if(WIN32)
    list(APPEND SOURCE_LIST ${CMAKE_CURRENT_LIST_DIR}/core/windows/core.cpp)
elseif(UNIX)
    list(APPEND SOURCE_LIST ${CMAKE_CURRENT_LIST_DIR}/core/unix/core.cpp)
else()
    message(FATAL_ERROR "Unknown system work on")
endif()

add_library(lib_loader ${SOURCE_LIST})
target_compile_features(lib_loader PRIVATE cxx_std_20)

target_include_directories(lib_loader PUBLIC
    ${PROJECT_SOURCE_DIR}/src/helpers/filesystem
    ${CMAKE_CURRENT_LIST_DIR}/core
)
