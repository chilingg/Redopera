cmake_minimum_required(VERSION 3.1)

add_executable(format format.cpp)
target_link_libraries(format redopera)

add_executable(unitTest unitTest.cpp)
target_link_libraries(unitTest redopera)

add_executable(name name.cpp)
target_link_libraries(name redopera)

add_executable(threads threads.cpp)
target_link_libraries(threads redopera)

add_executable(font font.cpp)
target_link_libraries(font redopera)

add_executable(opengl opengl.cpp)
target_link_libraries(opengl PRIVATE redopera)

add_executable(sound sound.cpp)
target_link_libraries(sound redopera)

add_executable(input input.cpp)
target_link_libraries(input redopera)

add_executable(context context.cpp)
target_link_libraries(context redopera)

find_file(RTEST_FILE rtest.cpp .)
if(RTEST_FILE)
    add_executable(rtest rtest.cpp)
    target_link_libraries(rtest redopera)
endif()

file(COPY TestFile/ DESTINATION ${EXECUTABLE_OUTPUT_PATH}TestFile)
