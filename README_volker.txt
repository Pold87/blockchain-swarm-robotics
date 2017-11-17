* Important things to remember
** Do NOT copy/paste the data from the browser to the data.txt file on the server; this copy&paste action modifies the data for some reason
** Check that the sent gas (in geth_static.cpp) is high enough

* The next geth version uses go instead of C for generating the DAG, therefore, I might get memory errors in the future

* If I get an error like [FATAL] Error initializing loop functions
[FATAL] Can't load library "build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so" even after trying to add extensions for shared library (so) and module library (so): 
/home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so: /home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so: cannot open shared object file: No such file or directory
/home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so.so: /home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so.so: cannot open shared object file: No such file or directory
/home/vstrobel/argos3-dist/lib/argos3/build/loop_functions/environment_classification_loop_functions/libenvironment_classification_loop_functions.so.so: OK
 it means that I declared something in the header file of the loop function but did not implement it in the cpp file.
