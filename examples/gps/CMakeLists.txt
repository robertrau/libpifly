cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ../../scripts/pi.cmake) 
project(example_gps)

include_directories(${libpifly_INCLUDE_DIRS})

file(GLOB SOURCES "*.cpp")

link_directories(${CMAKE_SYSROOT}/usr/local/lib)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${libpifly_LINK_LIBS})
