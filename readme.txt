This folder contains the source files for the environment classification. 

* loop_functions: it contains the .h and .cpp files about the loop functions. environment_classification_qt_user_functions are only used to draw the arena in Argos, while the actual loop functions set up and run the experiment, using the robots’ behaviour described by the controller. 

* experiments: it contains the configuration files for the local (epuck_EC_locale.argos) and for the cluster (c_epuck_environment_classification.argos) executions. 

* controllers/epuck_environment_classification: 
	- epuck_environment_classification.cpp and .h: controller of the robots
	- setup_tmp_dir cleanup user’s folder on the cluster’s nodes
	- vp_ec_exp1_dr1.sh: vary parameters script. It’s an example to vary parameters in order to launch multiple experiments with different parameters on the cluster
	
* The next geth version uses go instead of C for generating the DAG, therefore, I might get memory errors in the future
