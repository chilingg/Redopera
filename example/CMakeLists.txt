cmake_minimum_required(VERSION 3.1)

include_directories(${CMAKE_SOURCE_DIR}/include/redopera)

link_directories(${CMAKE_BINARY_DIR})

add_executable(triangle triangle.cpp)
target_link_libraries(triangle redopera)
set_target_properties(triangle PROPERTIES LINK_FLAGS_RELEASE "-mwindows")

add_executable(plane plane.cpp)
target_link_libraries(plane redopera)
set_target_properties(plane PROPERTIES LINK_FLAGS_RELEASE "-mwindows")

add_executable(texts texts.cpp)
target_link_libraries(texts redopera)
set_target_properties(texts PROPERTIES LINK_FLAGS_RELEASE "-mwindows")
