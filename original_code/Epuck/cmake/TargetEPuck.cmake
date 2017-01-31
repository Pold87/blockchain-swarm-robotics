#
# Use this file as follows:
# $ cmake -DCMAKE_TOOLCHAIN_FILE=TargetEPuck.cmake <other args...>
#

# The name of the target system
# 'Linux' here is fine because the target board has a Linux OS on it
# NOTE: When this variable is set, the variable CMAKE_CROSSCOMPILING
# is also set automatically by CMake
set(CMAKE_SYSTEM_NAME Linux)

# Full path to C compiler
set(CMAKE_C_COMPILER /usr/local/angstrom/arm/bin/arm-angstrom-linux-gnueabi-gcc)

# Full path to C++ compiler
set(CMAKE_CXX_COMPILER /usr/local/angstrom/arm/bin/arm-angstrom-linux-gnueabi-g++)

# Set the root directories for the find_* commands
# Configure CMake to search for headers and libraries only in those directories
set(CMAKE_FIND_ROOT_PATH /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# By default, install stuff in the toolchain tree
set(CMAKE_INSTALL_PREFIX ${CMAKE_FIND_ROOT_PATH}/usr)

# Configure ARGoS flags
set(ARGOS_BUILD_FOR epuck)
set(ARGOS_DOCUMENTATION OFF)
set(ARGOS_DYNAMIC_LIBRARY_LOADING OFF)
set(ARGOS_THREADSAFE_LOG OFF)
set(ARGOS_USE_DOUBLE OFF)
set(ARGOS_INCLUDE_DIRS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/include)
set(ARGOS_LIBRARY_DIRS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/lib/argos3)
