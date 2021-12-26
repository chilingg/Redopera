cmake_minimum_required(VERSION 3.5)

add_executable(triangle triangle.cpp)
target_link_libraries(triangle PRIVATE redopera)

add_executable(plane plane.cpp)
target_link_libraries(plane PRIVATE redopera)

add_executable(text text.cpp)
target_link_libraries(text PRIVATE redopera)
