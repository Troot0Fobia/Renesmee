cmake_minimum_required(VERSION 3.20)

if(WIN32)
    set(LIB_LOADER_SOURCE_FILE ${CMAKE_CURRENT_LIST_DIR}/windows/lib_loader.cpp)
elseif(UNIX)
    set(LIB_LOADER_SOURCE_FILE ${CMAKE_CURRENT_LIST_DIR}/unix/lib_loader.cpp)
else()
    message(FATAL_ERROR "Unknown system work on")
endif()

add_library(lib_loader STATIC ${LIB_LOADER_SOURCE_FILE})
target_compile_features(lib_loader PRIVATE cxx_std_20)

target_include_directories(lib_loader PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
