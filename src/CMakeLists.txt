cmake_minimum_required(VERSION 3.1)

aux_source_directory(. REDOPERA_SRC)
aux_source_directory(dependent REDOPERA_SRC)

option(BUILD_SHARED_LIBS "Build as shared library" ON)

if(BUILD_SHARED_LIBS STREQUAL "ON")
    add_library(redopera STATIC ${REDOPERA_SRC})    # 静态库
else()
    add_library(redopera SHARED ${REDOPERA_SRC})    # 动态库
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(redopera dl glfw pthread lua rtaudio)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(redopera glfw3 lua rtaudio ksuser mfplat mfuuid wmcodecdspuuid)
endif()

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET_TARGET_PROPERTIES(redopera PROPERTIES OUTPUT_NAME redoperad)
else()
    add_definitions(-DNDEBUG)
endif()
