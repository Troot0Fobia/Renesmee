cmake_minimum_required(VERSION 3.20)

project(Renesmee)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cxxopts INTERFACE)
add_library(signal_handler STATIC ${CMAKE_SOURCE_DIR}/libs/ctrl_c_handler/ctrl_c.cpp)
target_include_directories(cxxopts INTERFACE ${CMAKE_SOURCE_DIR}/libs/cxxopts)
target_include_directories(signal_handler PUBLIC ${CMAKE_SOURCE_DIR}/libs/ctrl_c_handler)

add_subdirectory(src/Console)
add_subdirectory(src/Renesmee)
add_subdirectory(src/Network)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(cpr REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE console renesmee network cpr::cpr signal_handler)
