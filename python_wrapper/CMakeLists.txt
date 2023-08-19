cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ../scripts/pi.cmake) 
project(pifly_python)

file(GLOB SOURCES "./*.cpp")

#include_directories(${lib${PROJECT_NAME}_INCLUDE_DIRS})
include_directories(${libpifly_INCLUDE_DIRS} ${CMAKE_SYSROOT}/usr/include/python3.9)

#add_compile_options(-pthread -std=c++11)

link_directories(${libpifly_LINK_DIRS})
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${libpifly_LINK_LIBS} boost_python39 python3.9)
