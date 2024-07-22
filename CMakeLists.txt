cmake_minimum_required(VERSION 3.22)
project(OpenSA VERSION 0.0.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(OpenSA MODULE)

if (CMAKE_DEBUG_TYPE STREQUAL Debug)
    set(OPENSA_DEBUG True)
else()
    set_target_properties(OpenSA PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

configure_file(sa_config.h.in sa_config.h)

add_subdirectory(vendor/branchrt)

set(RAKNET_ENABLE_DLL True)
add_subdirectory(vendor/RakNet)
target_include_directories(OpenSA PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${RAKNET_HEADER_SRCS})
target_sources(OpenSA PRIVATE 
    plugin_load_thread.cpp 
    opensa_logger.cpp 
    virtual_env.cpp
    game/game_context.cpp 
    game/hooks_cortex.cpp
    game/main_menu_hooks.cpp
    game/texture_runtime.cpp

    address_map.cpp
    sadir_logfile.cpp
    )

target_link_libraries(OpenSA branchrt log)

install(TARGETS OpenSA LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/output)
install(TARGETS RakNet LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/output)
install(FILES ${CMAKE_SOURCE_DIR}/runtime/libc++_shared.so DESTINATION ${CMAKE_SOURCE_DIR}/output)
