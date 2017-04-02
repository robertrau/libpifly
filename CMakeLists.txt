cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ./scripts/pi.cmake) 
project(libpifly)

# Need to write a find module for this
#find_package(bcm2835 REQUIRED)

file(GLOB SOURCES "src/*/*.c")

set(${PROJECT_NAME}_INCLUDE_DIRS include 
								 ${CMAKE_SYSROOT}/usr/local/include
								 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

add_subdirectory(examples/adc)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
