cmake_minimum_required(VERSION 3.4.1)

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" ) # -Werror
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate" )
set( LIB_NAME "native-lib" )

set( GAME_SRC
        ../../../../../src/framework/App.h
        ../../../../../src/framework/App.cc
        ../../../../../src/framework/modules/Renderer.h
        ../../../../../src/framework/modules/Renderer.cc
        ../../../../../src/framework/assets/ResourceManager.h
        ../../../../../src/framework/assets/ResourceManager.cc
        ../../../../../src/framework/utils/Log.h
        ../../../../../src/framework/utils/Log.cc
        ../../../../../src/Config.h
        ../../../../../src/framework/Types.h
        ../../../../../src/framework/platforms/AndroidAppState.h
        ../../../../../src/framework/platforms/AndroidAppState.cc
        ../../../../../src/framework/platforms/android.cc
        ../../../../../src/Realm.h
        ../../../../../src/Realm.cc
        )

set( ANDROID_GLUE_SRC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c )

set( STB_HEADERS ../../../../../3rdparty/stb/ )

add_library( android_app_glue STATIC ${ANDROID_GLUE_SRC} )

add_library( ${LIB_NAME} SHARED ${GAME_SRC} )

find_library(
        log-lib
        log)

target_compile_options( ${LIB_NAME} PUBLIC "-O3" )

target_include_directories( ${LIB_NAME} PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${STB_HEADERS}
        )

target_link_libraries( ${LIB_NAME}
        android
        android_app_glue
        EGL
        GLESv2
        ${log-lib}
        )
