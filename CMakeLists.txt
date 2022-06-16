cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE ./scripts/pi.cmake) 
project(pifly)

# Need to write a find module for this
#find_package(bcm2835 REQUIRED)

file(GLOB SOURCES "src/*/*.cpp" "src/*/*/*.cpp")

set(lib${PROJECT_NAME}_INCLUDE_DIRS include 
								 ${CMAKE_SYSROOT}/usr/local/include
								 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(lib${PROJECT_NAME}_LINK_DIRS ${CMAKE_SYSROOT}/usr/local/lib 
								 ${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf/
								 CACHE INTERNAL "${PROJECT_NAME}: Link Directories" FORCE)

set(lib${PROJECT_NAME}_LINK_LIBS pthread
							  pifly
							  bcm2835 
							  CACHE INTERNAL "${PROJECT_NAME}: Link libs" FORCE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Weffc++ -pedantic -D__ARM_SIZEOF_WCHAR_T=32 -D__ARM_FEATURE_SAT=1 -D__ARM_ARCH_ISA_ARM=1 -D__ARMEL__=1 -D__ARM_FEATURE_UNALIGNED=1 -D__ARM_FP=12 -D__ARM_NEON_FP=4 -D__ARM_SIZEOF_MINIMAL_ENUM=4 -D__ARM_PCS_VFP=1 -D__ARM_FEATURE_LDREX=4 -D__ARM_FEATURE_QBIT=1 -D__ARM_ARCH_6__=1 -D__ARM_32BIT_STATE=1 -D__ARM_FEATURE_CLZ=1 -D__ARM_ARCH_ISA_THUMB=1 -D__ARM_ARCH=6 -D__ARM_FEATURE_SIMD32=1 -D__ARM_EABI__=1 -D__ARM_FEATURE_DSP=1 -U__x86_64__")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Weffc++ -pedantic")

find_program(
	CLANG_TIDY_EXE
	NAMES "clang-tidy"
	DOC "Path to clang-tidy executable"
	)
if(NOT CLANG_TIDY_EXE)
	message(STATUS "clang-tidy not found.")
else()
	message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
	set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
		"-system-headers=0;-header-filter=./include;-checks=-*,clang-analyzer-*,modernize-*,cppcoreguidelines-*,bugprone-*,-clang-analyzer-alpha.*,-cppcoreguidelines-pro-bounds-constant-array-index,-modernize-deprecated-headers"
	)
endif()

include_directories(${lib${PROJECT_NAME}_INCLUDE_DIRS}
	${CMAKE_SYSROOT}/usr/include/c++/4.9
	${CMAKE_SYSROOT}/usr/include/arm-linux-gnueabihf
	${CMAKE_SYSROOT}/usr/include/arm-linux-gnueabihf/c++/4.9
	${CMAKE_SYSROOT}/usr/lib/gcc/arm-linux-gnueabihf/4.9/include
	${CMAKE_SYSROOT}/usr/include
)

add_compile_options(-pthread -std=c++11)

add_subdirectory(examples/adc)
add_subdirectory(examples/gps)
add_subdirectory(python_wrapper)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} pthread bcm2835)

if(CLANG_TIDY_EXE)
  set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
  )
endif()
