cmake_minimum_required(VERSION 3.22)
project(GSdxGLES30 LANGUAGES C CXX)

if(NOT ANDROID)
    message(FATAL_ERROR "GSdxGLES30 is an Android-only target.")
endif()

add_library(gsdx_gles30 STATIC src/AndroidGLESContext.cpp)
target_include_directories(gsdx_gles30 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(gsdx_gles30 PUBLIC cxx_std_17)
target_compile_definitions(gsdx_gles30 PUBLIC GSdx_ANDROID=1 GSdx_GLES_VERSION=300)

find_library(ANDROID_LIB android REQUIRED)
find_library(EGL_LIB EGL REQUIRED)
find_library(GLES_LIB GLESv3 REQUIRED)
find_library(LOG_LIB log REQUIRED)
target_link_libraries(gsdx_gles30 PUBLIC ${ANDROID_LIB} ${EGL_LIB} ${GLES_LIB} ${LOG_LIB})

# Configure Gradle/NDK abiFilters to armeabi-v7a and arm64-v8a.
# This target deliberately does not link desktop OpenGL.