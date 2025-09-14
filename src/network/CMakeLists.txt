cmake_minimum_required(VERSION 3.20)
include(FetchContent)

find_package(cpr QUIET)

if((cpr_FOUND) AND (${cpr_VERSION} VERSION_GREATER_EQUAL "1.12.0"))
    message(STATUS "Found cpr version ${cpr_VERSION}")
else()
    message(STATUS "cpr not found. Fetching from source")
    FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.12.0)
    FetchContent_MakeAvailable(cpr)
endif()

add_library(network network.cpp network.hpp)

target_link_libraries(network PUBLIC cpr::cpr)
