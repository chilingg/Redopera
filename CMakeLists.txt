cmake_minimum_required(VERSION 3.1)

project(Redopera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/redopera)

ADD_SUBDIRECTORY(test test)
ADD_SUBDIRECTORY(src src)
ADD_SUBDIRECTORY(example example)

find_file(VARIETY CMakeLists.txt variety)
if(VARIETY)
    set(VARIETY_SUBMODULE ${CMAKE_BINARY_DIR}/src)
    set(REDOPERA_LIB_PATH ${CMAKE_BINARY_DIR}/src)
    set(REDOPERA_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
    ADD_SUBDIRECTORY(variety variety)
endif()
