cmake_minimum_required(VERSION 3.22)

project(
	NPCsUsePotions
	VERSION 3.1.4.1
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_BUILD_PARALLEL_LEVEL ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	message(FATAL_ERROR "in-source builds are not allowed")
endif()

macro(set_from_environment VARIABLE)
	if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif()
endmacro()

set_from_environment(SkyrimOutputPath)
if(NOT DEFINED SkyrimOutputPath)
	message(FATAL_ERROR "SkyrimOutputPath is not set")
endif()

option(COPY_OUTPUT "copy the output of build operations to the game directory" OFF)
option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE in the dynamic runtime feature." OFF)
option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE in the dynamic runtime feature." OFF)
option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR in the dynamic runtime feature." ON)
set(BUILD_TESTS OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(src)
include(cmake/packaging.cmake)
