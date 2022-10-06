cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(OpenSA)

add_subdirectory(lib/hookrta64)

add_library(opensa SHARED plugin_load_thread.cpp)
target_link_libraries(opensa hookrta64)

