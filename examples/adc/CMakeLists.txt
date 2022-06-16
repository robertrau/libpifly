cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ../../scripts/pi.cmake) 
project(example_adc)

include_directories(${libpifly_INCLUDE_DIRS})

file(GLOB SOURCES "*.cpp")

link_directories(${libpifly_LINK_DIRS})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${libpifly_LINK_LIBS})

if(CLANG_TIDY_EXE)
  set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif()
