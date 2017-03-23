cmake_minimum_required(VERSION 3.4)

project(GENERIC_CAMERA)

set(CMAKE_CXX_STANDARD 11)
set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")


set(ABSTRACT_CAMERA_LIB abstract_camera)
aux_source_directory(./camera/ ABSTRACT_CAMERA_SRC)
add_library(${ABSTRACT_CAMERA_LIB} SHARED ${ABSTRACT_CAMERA_SRC})
