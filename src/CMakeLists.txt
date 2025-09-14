cmake_minimum_required(VERSION 3.20)

add_subdirectory(console)
add_subdirectory(network)
add_subdirectory(application)

add_library(signal_handler STATIC ${CMAKE_SOURCE_DIR}/libs/ctrl_c_handler/ctrl_c.cpp)
target_include_directories(signal_handler PUBLIC ${CMAKE_SOURCE_DIR}/libs/ctrl_c_handler)

add_executable(Renesmee main.cpp)
target_link_libraries(Renesmee PUBLIC application console network signal_handler)
