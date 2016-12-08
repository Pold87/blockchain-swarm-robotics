#
# What is ARGoS being built for?
# Accepted values: "simulator" or a robot name (lowercase)
#
if(NOT DEFINED ARGOS_BUILD_FOR)
  # Variable was not set, set to default value
  set(ARGOS_BUILD_FOR "simulator" CACHE STRING "What is ARGoS being built for? \"simulator\" or a robot name (lowercase)")
else(NOT DEFINED ARGOS_BUILD_FOR)
  # Variable was set, make it public
  set(ARGOS_BUILD_FOR ${ARGOS_BUILD_FOR} CACHE STRING "What is ARGoS being built for? \"simulator\" or a robot name (lowercase)")
endif(NOT DEFINED ARGOS_BUILD_FOR)
# Set a macro according to value set in ARGOS_BUILD_FOR
add_definitions(-DARGOS_${ARGOS_BUILD_FOR}_BUILD)
#
# Create convenience variables for checks in the CMake files
#
set(ARGOS_BUILD_FOR_SIMULATOR  FALSE)
set(ARGOS_BUILD_FOR_EPUCK      FALSE)
set(ARGOS_BUILD_FOR_LOCALEPUCK FALSE)
# ARGOS_BUILD_FOR_SIMULATOR
if(ARGOS_BUILD_FOR STREQUAL "simulator")
  set(ARGOS_BUILD_FOR_SIMULATOR TRUE)
endif(ARGOS_BUILD_FOR STREQUAL "simulator")
# ARGOS_BUILD_FOR_EPUCK
if(ARGOS_BUILD_FOR STREQUAL "epuck")
  set(ARGOS_BUILD_FOR_EPUCK TRUE)
endif(ARGOS_BUILD_FOR STREQUAL "epuck")
# ARGOS_BUILD_FOR_LOCALEPUCK
if(ARGOS_BUILD_FOR STREQUAL "localepuck")
  set(ARGOS_BUILD_FOR_LOCALEPUCK TRUE)
endif(ARGOS_BUILD_FOR STREQUAL "localepuck")
