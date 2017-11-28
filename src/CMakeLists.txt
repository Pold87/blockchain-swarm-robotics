cmake_minimum_required(VERSION 2.8)
project(argos3-blockchain-collective-decision)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the path where CMake will find additional scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Include path points to the base source dir
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

# if(ARGOS_BUILD_FOR STREQUAL "simulator")
# 	set (ARGOS_BUILD_FOR_SIMULATOR)

# 	# Find the ARGoS package
# 	find_package(PkgConfig)
# 	pkg_check_modules(ARGOS REQUIRED argos3_simulator)
# 	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)
# 	message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

# 	# Check whether all the necessary libs have been installed to compile the
# 	# code that depends on Qt and OpenGL
# 	include(ARGoSCheckQTOpenGL)

# 	# Find the GSL package
# 	find_package(GSL)

# 	# Find Lua
# 	find_package(Lua52)
# else()
# 	set (ARGOS_BUILD_FOR_EPUCK)
# endif(ARGOS_BUILD_FOR STREQUAL "simulator")

set (ARGOS_BUILD_FOR_EPUCK)


# Make sure we are under Unix
if(NOT UNIX)
  message(FATAL_ERROR "This plugin compiles only under UNIX, sorry!")
endif(NOT UNIX)

# Set build options
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildOptions.cmake)

# Set compiler flags
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildFlags.cmake)

# Check for libraries
include(${CMAKE_SOURCE_DIR}/cmake/ARGoSBuildChecks.cmake)


# Set up CPack for later use
#include(${CMAKE_SOURCE_DIR}/cmake/ARGoSPackaging.cmake)

# Set up ARGoS compilation information
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIRS})

# Descend into the controllers directory
add_subdirectory(controllers)

# Descend into the loop_functions directory
#add_subdirectory(loop_functions)

