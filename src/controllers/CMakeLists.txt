MESSAGE("Cmaking controllers")

if(ARGOS_BUILD_FOR_SIMULATOR)

add_library(epuck_environment_classification SHARED 
	    epuck_environment_classification.h 
            epuck_environment_classification.cpp
	    geth_static.h
	    geth_static.cpp)

target_link_libraries(epuck_environment_classification
  argos3core_simulator
  argos3plugin_simulator_epuck
  argos3plugin_simulator_genericrobot)

else()

  if(ARGOS_BUILD_FOR STREQUAL "epuck")

	  MESSAGE("Cmaking for real epucks")

	  add_executable(epuck_blockchain
      epuck_environment_classification.h 
      epuck_environment_classification.cpp
      geth_static.h
      geth_static.cpp
      )
    
    target_link_libraries(epuck_blockchain
      argos3core_${ARGOS_BUILD_FOR}
      argos3plugin_${ARGOS_BUILD_FOR}_epuck
      argos3plugin_${ARGOS_BUILD_FOR}_genericrobot
      argos3plugin_${ARGOS_BUILD_FOR}_genericvirtualsensorsandactuators)
    
    
  else(ARGOS_BUILD_FOR STREQUAL "epuck")
    MESSAGE("TODO compilation for target != epuck and != simulator")
  endif(ARGOS_BUILD_FOR STREQUAL "epuck")  
  
endif()
