add_library(epuck_environment_classification SHARED 
	    epuck_environment_classification.h 
            epuck_environment_classification.cpp)
target_link_libraries(epuck_environment_classification
  argos3core_simulator
  argos3plugin_simulator_epuck
  argos3plugin_simulator_genericrobot)
