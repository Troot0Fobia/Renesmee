cmake_minimum_required(VERSION 3.20)

set(CONTROLLERS_FOLDER ${CMAKE_CURRENT_LIST_DIR}/controllers)

if(WIN32)
    set(SIGNAL_HANDLER_SOURCE_FILE ${CONTROLLERS_FOLDER}/signal_handler/windows/signal_handler.cpp)
    set(RENDERER_SOURCE_FILE ${CONTROLLERS_FOLDER}/constollers/console_renderer/windows/renderer.cpp)
elseif(UNIX)
    set(SIGNAL_HANDLER_SOURCE_FILE ${CONTROLLERS_FOLDER}/signal_handler/unix/signal_handler.cpp)
    set(RENDERER_SOURCE_FILE ${CONTROLLERS_FOLDER}/console_renderer/unix/renderer.cpp)
else()
    message(FATAL_ERROR "Unknown system work on")
endif()

add_library(adapters STATIC
    models/config/config.cpp
    models/console/console.cpp
    models/plugin/plugin.cpp

    ${SIGNAL_HANDLER_SOURCE_FILE}
    ${RENDERER_SOURCE_FILE}
)
target_compile_features(adapters PUBLIC cxx_std_20)

target_include_directories(adapters PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/models/config
    ${CMAKE_CURRENT_SOURCE_DIR}/models/console
    ${CMAKE_CURRENT_SOURCE_DIR}/models/plugin

    ${CONTROLLERS_FOLDER}/console_renderer
    ${CONTROLLERS_FOLDER}/signal_handler

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
