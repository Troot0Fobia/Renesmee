cmake_minimum_required(VERSION 3.20)

add_subdirectory(domain)
get_target_property(DOMAIN_INCLUDES domain INTERFACE_INCLUDE_DIRECTORIES)

add_subdirectory(lib_loader)
get_target_property(LOADER_INCLUDES lib_loader INTERFACE_INCLUDE_DIRECTORIES)

add_subdirectory(utils)
get_target_property(UTILS_INCLUDES utils INTERFACE_INCLUDE_DIRECTORIES)

add_subdirectory(adapters)
get_target_property(ADAPTERS_INCLUDES adapters INTERFACE_INCLUDE_DIRECTORIES)

add_executable(RenesmeeCLI main.cpp)
target_compile_features(RenesmeeCLI PUBLIC cxx_std_20)

target_link_libraries(RenesmeeCLI PRIVATE
    domain
    lib_loader
    utils
    adapters
)

target_include_directories(RenesmeeCLI PRIVATE
    ${DOMAIN_INCLUDES}
    ${LOADER_INCLUDES}
    ${UTILS_INCLUDES}
    ${ADAPTERS_INCLUDES}
)
