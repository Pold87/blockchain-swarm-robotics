#
# Use this file as follows:
# $ cmake -DCMAKE_TOOLCHAIN_FILE=TargetLocalEPuck.cmake <other args...>
#

# The name of the target system
# 'Linux' here is fine because the target board has a Linux OS on it
# NOTE: When this variable is set, the variable CMAKE_CROSSCOMPILING
# is also set automatically by CMake
#set(CMAKE_SYSTEM_NAME Linux)

# Configure ARGoS flags
set(ARGOS_BUILD_FOR localepuck)
set(ARGOS_DOCUMENTATION OFF)
set(ARGOS_DYNAMIC_LIBRARY_LOADING OFF)
set(ARGOS_THREADSAFE_LOG OFF)
set(ARGOS_USE_DOUBLE OFF)

