cmake_minimum_required(VERSION 3.1)

project(Redopera LANGUAGES CXX)

ADD_SUBDIRECTORY(test test)
ADD_SUBDIRECTORY(src src)
ADD_SUBDIRECTORY(example example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
