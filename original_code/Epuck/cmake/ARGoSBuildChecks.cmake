#
# Find the ARGoS package
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  find_package(PkgConfig)
  pkg_check_modules(ARGOS REQUIRED argos3_simulator)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)
  include_directories(${ARGOS_INCLUDE_DIRS})
  link_directories(${ARGOS_LIBRARY_DIRS})
elseif(ARGOS_BUILD_FOR_LOCALEPUCK)
  find_package(PkgConfig)
  pkg_check_modules(ARGOS REQUIRED argos3_localepuck)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)
  include_directories(${ARGOS_INCLUDE_DIRS})
  link_directories(${ARGOS_LIBRARY_DIRS})
endif()

#
# Check for Qt and OpenGL when compiling for the simulator
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  include(ARGoSCheckQTOpenGL)
  find_package(Lua52)
  if(LUA52_FOUND)
    include_directories(${LUA_INCLUDE_DIR})
  endif(LUA52_FOUND)
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Check for Qt when compiling for the local e-puck tools
#
if(ARGOS_BUILD_FOR_LOCALEPUCK)
  find_package(Qt4 REQUIRED QtCore QtGui)
endif(ARGOS_BUILD_FOR_LOCALEPUCK)

#
# Check for PThreads
# It is required only when compiling the real robot
#
if(NOT ARGOS_BUILD_FOR_SIMULATOR)
  find_package(Pthreads)
  if(NOT PTHREADS_FOUND)  
    message(FATAL_ERROR "Required library pthreads not found.")
  endif(NOT PTHREADS_FOUND)
  add_definitions(${PTHREADS_DEFINITIONS})
  include_directories(${PTHREADS_INCLUDE_DIR})
endif(NOT ARGOS_BUILD_FOR_SIMULATOR)
