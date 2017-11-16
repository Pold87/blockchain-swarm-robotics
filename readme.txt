# Managing Byzantine Robots via Blockchain Technology in a Swarm Robotics Collective Decision Making Scenario



This folder contains the source files for the environment classification. 

* loop_functions: it contains the .h and .cpp files about the loop functions. environment_classification_qt_user_functions are only used to draw the arena in Argos, while the actual loop functions set up and run the experiment, using the robots’ behaviour described by the controller. 

* experiments: it contains the configuration files for the local (epuck_EC_locale.argos) and for the cluster (c_epuck_environment_classification.argos) executions. 

* controllers/epuck_environment_classification: 
	- epuck_environment_classification.cpp and .h: controller of the robots
	- setup_tmp_dir cleanup user’s folder on the cluster’s nodes
	- vp_ec_exp1_dr1.sh: vary parameters script. It’s an example to vary parameters in order to launch multiple experiments with different parameters on the cluster
	
* The next geth version uses go instead of C for generating the DAG, therefore, I might get memory errors in the future

* If I get an error like [FATAL] Error initializing loop functions
[FATAL] Can't load library "build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so" even after trying to add extensions for shared library (so) and module library (so): 
/home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so: /home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so: cannot open shared object file: No such file or directory
/home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so.so: /home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so.so: cannot open shared object file: No such file or directory
/home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so.so: OK
 it means that I declared something in the header file of the loop function but did not implement it in the cpp file.
