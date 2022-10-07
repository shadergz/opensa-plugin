cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(OpenSA)

add_subdirectory(lib/hookrta64)
include_directories(${CMAKE_SOURCE_DIR})

add_library(opensa SHARED plugin_load_thread.cpp opensa_logger.cpp)
target_link_libraries(opensa hookrta64 log)

