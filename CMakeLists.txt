cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ./scripts/pi.cmake) 
project(pifly)

# Need to write a find module for this
#find_package(bcm2835 REQUIRED)

file(GLOB SOURCES "src/*/*.cpp" "src/*/*/*.cpp")

set(lib${PROJECT_NAME}_INCLUDE_DIRS include 
								 ${CMAKE_SYSROOT}/usr/local/include
								 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(lib${PROJECT_NAME}_LINK_LIBS pthread
							  pifly
							  bcm2835 
							  CACHE INTERNAL "${PROJECT_NAME}: Link libs" FORCE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

include_directories(${lib${PROJECT_NAME}_INCLUDE_DIRS})

add_compile_options(-pthread -std=c++11)

add_subdirectory(examples/adc)
add_subdirectory(examples/gps)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
