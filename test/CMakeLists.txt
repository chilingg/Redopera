cmake_minimum_required(VERSION 3.1)

include_directories(${CMAKE_SOURCE_DIR}/include/redopera)

link_directories(${CMAKE_BINARY_DIR}/src)

add_executable(sound sound.cpp)
target_link_libraries(sound redopera)

add_executable(threads threads.cpp)
target_link_libraries(threads redopera)

add_executable(window window.cpp)
target_link_libraries(window redopera)

add_executable(font font.cpp)
target_link_libraries(font redopera)

add_executable(glContex glContext.cpp)
target_link_libraries(glContex redopera)

add_executable(opengl opengl.cpp)
target_link_libraries(opengl redopera)

add_executable(share share.cpp)
target_link_libraries(share redopera)

add_executable(input input.cpp)
target_link_libraries(input redopera)

add_executable(unitTest unitTest.cpp)
target_link_libraries(unitTest redopera)

find_file(R_TEST rtest.cpp .)
if(R_TEST)
    add_executable(rtest rtest.cpp)
    target_link_libraries(rtest redopera)
endif()

file(COPY TestFile/ DESTINATION ${CMAKE_BINARY_DIR}/test/TestFile)
