cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ./scripts/pi.cmake) 
project(libpifly)

# Need to write a find module for this
#find_package(bcm2835 REQUIRED)

file(GLOB SOURCES "src/*/*.cpp")

set(${PROJECT_NAME}_INCLUDE_DIRS include 
								 ${CMAKE_SYSROOT}/usr/local/include
								 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(${PROJECT_NAME}_LINK_LIBS pthread
							  bcm2835
							  libpifly 
							  CACHE INTERNAL "${PROJECT_NAME}: Link libs" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

add_compile_options(-pthread -std=c++11)

add_subdirectory(examples/adc)
add_subdirectory(examples/gps)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
