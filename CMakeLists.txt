cmake_minimum_required(VERSION 3.15)

project(Redopera VERSION 1.4 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(extern extern)
add_subdirectory(src src)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    add_subdirectory(test test)
    add_subdirectory(example example)
endif()
