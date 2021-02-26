cmake_minimum_required(VERSION 3.1)

link_directories(${CMAKE_BINARY_DIR})

add_executable(debugInput debugInput.cpp)
target_link_libraries(debugInput redopera)

add_executable(sound sound.cpp)
target_link_libraries(sound redopera)

add_executable(threads threads.cpp)
target_link_libraries(threads redopera)

add_executable(font font.cpp)
target_link_libraries(font redopera)

add_executable(glContext glContext.cpp)
target_link_libraries(glContext redopera)

add_executable(event event.cpp)
target_link_libraries(event redopera)

add_executable(opengl opengl.cpp)
target_link_libraries(opengl redopera)

add_executable(window window.cpp)
target_link_libraries(window redopera)

add_executable(unitTest unitTest.cpp)
target_link_libraries(unitTest redopera)

add_executable(name name.cpp)
target_link_libraries(name redopera)

find_file(RTEST_FILE rtest.cpp .)
if(RTEST_FILE)
    add_executable(rtest rtest.cpp)
    target_link_libraries(rtest redopera)
endif()

file(COPY TestFile/ DESTINATION ${EXECUTABLE_OUTPUT_PATH}TestFile)
