cmake_minimum_required(VERSION 3.18)
project(OpenSA VERSION 0.0.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(OPENSA_DEBUG "Enable developer mode" False)

if (${OPENSA_DEBUG} EQUAL True)
    set(HOOK_RT_DEBUG True)
endif()

configure_file(SA_Config.h.in SA_Config.h)

add_subdirectory(vendor/bra64rt)

set(RAKNET_ENABLE_DLL True)
add_subdirectory(vendor/RakNet)

add_library(OpenSA MODULE)

target_include_directories(OpenSA PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${RAKNET_HEADER_SRCS})
target_sources(OpenSA PRIVATE 
    plugin_load_thread.cpp 
    opensa_logger.cpp 
    Virtual_ENV.cpp
    game/Game_Context.cpp 
    game/Hooks_Cortex.cpp
    game/MainMenu_Hooks.cpp)

target_link_libraries(OpenSA bra64rt log)

install(TARGETS OpenSA LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/output)
install(TARGETS RakNet LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/output)
install(FILES ${CMAKE_SOURCE_DIR}/runtime/libc++_shared.so DESTINATION ${CMAKE_SOURCE_DIR}/output)
