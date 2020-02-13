cmake_minimum_required(VERSION 3.1)
#SET(REDOPERA_SRC )

aux_source_directory(. REDOPERA_SRC)
aux_source_directory(dependent/ REDOPERA_SRC)

option(BUILD_SHARED_LIBS "Build as shared library" ON)

if(BUILD_SHARED_LIBS STREQUAL "ON")
    add_library(redopera STATIC ${REDOPERA_SRC})    # 静态库
else()
    add_library(redopera SHARED ${REDOPERA_SRC})    # 动态库
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/redopera)

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DR_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(redopera pthread rtaudio glfw dl lua)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(redopera rtaudio ksuser mfplat mfuuid wmcodecdspuuid winmm ole32 glfw3 lua)
endif()
