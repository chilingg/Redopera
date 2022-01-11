cmake_minimum_required(VERSION 3.15)

option(BUILD_SHARED_LIBS "Build as shared library" ON)

set(REDOPERA_HEADERS
    ../include/redopera/RFormat.h
    ../include/redopera/RColor.h
    ../include/redopera/RPoint.h
    ../include/redopera/RSize.h
    ../include/redopera/RRect.h
    ../include/redopera/RTimer.h
    ../include/redopera/RName.h
    ../include/redopera/RMath.h
    ../include/redopera/RDefine.h
    ../include/redopera/RFunction.h
    ../include/redopera/RSigslot.h
    ../include/redopera/RKeeper.h
    ../include/redopera/RAudio.h
    ../include/redopera/RInput.h
    ../include/redopera/RGame.h
    ../include/redopera/RWindow.h
    ../include/redopera/RTextLoader.h
    ../include/redopera/REntity.h
    ../include/redopera/RDict.h

    ../include/redopera/rsc/RResource.h
    ../include/redopera/rsc/RImage.h
    ../include/redopera/rsc/RFile.h
    ../include/redopera/rsc/RFont.h
    ../include/redopera/rsc/RPack.h
    ../include/redopera/rsc/RScript.h
    ../include/redopera/rsc/RMusic.h
    ../include/redopera/rsc/RSoundEffect.h

    ../include/redopera/render/ROpenGL.h
    ../include/redopera/render/RShader.h
    ../include/redopera/render/RProgram.h
    ../include/redopera/render/RTexture.h
    ../include/redopera/render/RContext.h
    ../include/redopera/render/RRenderer.h
    ../include/redopera/render/RSprite.h
    ../include/redopera/render/render.h

    ../include/redopera/thread/RThread.h
    ../include/redopera/thread/RThreadPool.h
    ../include/redopera/thread/RThreadQueue.h
    ../include/redopera/thread/RThreadStack.h
    )
set(REDOPERA_SOURCES
    define.cpp
    RMath.cpp
    RImage.cpp
    RFont.cpp
    RPack.cpp
    RScript.cpp
    RShader.cpp
    RProgram.cpp
    RTexture.cpp
    RThreadPool.cpp
    RGame.cpp
    RWindow.cpp
    RContext.cpp
    RMusic.cpp
    RSoundEffect.cpp
    RInput.cpp
    RRenderer.cpp
    RTextLoader.cpp
    RSprite.cpp

    ../extern/glad.c
    ../extern/SourceCodePro.cpp
    )
add_library(redopera ${REDOPERA_HEADERS} ${REDOPERA_SOURCES})

target_include_directories(redopera PUBLIC ../include/redopera)
target_include_directories(redopera PRIVATE ../include/redopera/extern)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(redopera PUBLIC fmt pthread SDL2 SDL2_mixer)
    target_link_libraries(redopera PRIVATE lua dl)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # target_link_libraries(redopera -Wl,-Bstatic -lstdc++ -lgcc_eh -lpthread -Wl,-Bdynamic) # 链接静态库
endif()

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(redopera PROPERTIES OUTPUT_NAME redoperad)
else()
    target_compile_definitions(redopera PRIVATE NDEBUG)
endif()
