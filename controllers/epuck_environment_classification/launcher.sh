#!/bin/bash
#$ -N JOBPREFIXJOBCOUNTER
#$ -cwd
#$ -o /tmp/USERNAME/JOBPREFIXJOBCOUNTER.out
#$ -e /tmp/USERNAME/JOBPREFIXJOBCOUNTER.err
##$ -l long  ## Used for second queue option 
QUEUE_TYPE

###################################################################
# Script parameters/constants (not related to the job)            #
###################################################################

# Job counter
JCOUNTER=JOBPREFIXJOBCOUNTER
# Directory where the results are moved after the job esecution
RESULTSDIR=RESDIR
# Working directory for the job 
#WORKDIR="/tmp/USERNAME/JOBPREFIXJOBCOUNTER"
WORKDIR="/home/volker/tmpARGoS/"
# Job outputs dir (.out and .err files)
JOBDIR=JDIR
# Boolean flag to export argos (0) or not (!=0)
EXPORT_ARGOS=EXPORTARGOS
# Command to be executed (without possible parameters)
COMMAND=THECOMMAND

###################################################################
# The actual script running the job                               #
###################################################################

# Export argos if necessary
if [ $EXPORT_ARGOS -eq 0 ]
then
  # Configure ARGOSINSTALLDIR
  #export ARGOSINSTALLDIR="/home/USERNAME/argos2/"
  source "/lustre/home/dbrambilla/argos3-dist/bin/setup_argos3"
fi

# Create working dir (only for result files)
mkdir $WORKDIR
cd $WORKDIR

# Run the command passing all the arguments
${COMMAND} $@

# Move all the result files back to the home
nfiles=`ls -1 | wc -l`
if [ $nfiles -eq 0 ]
then
  # Configure ARGOSINSTALLDIR
  echo "Job $JCOUNTER did not produce output files!"
fi
mv -f * $RESULTSDIR

# Move the .out and .err files to the home
cd ..
mv -f $JCOUNTER.out $JOBDIR
mv -f $JCOUNTER.err $JOBDIR

# Remove the working directory
rm -rf $WORKDIR
