cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(OpenSA)

option(_OPENSA_DEBUG_ "Enable developer mode" False)

if (${_OPENSA_DEBUG_} STREQUAL True)
    set(_HOOK_RT_DEBUG_ True)
endif()

configure_file(sa_config.h.in sa_config.h)

add_subdirectory(lib/hookRuntimeA64)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(OpenSA SHARED plugin_load_thread.cpp 
    opensa_logger.cpp 
    Virtual_ENV.cpp
    game/Game_Context.cpp 
    game/Hooks_Cortex.cpp
    game/MainMenu_Hooks.cpp)
target_link_libraries(OpenSA hookRuntimeA64 log)

