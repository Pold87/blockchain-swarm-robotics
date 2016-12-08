#!/bin/bash

################################################################################
################################################################################

# Usage:
# qwait <number_jobs=3400>
function qwait {
  EXPECTED_ARGS=1
  JOBS=3400
  E_BADARGS=65
  USERNAME=`whoami`
  TO_SLEEP=300
    
  if [ $# -eq $EXPECTED_ARGS ]
  then
    JOBS=$1
  fi
    
  if [ $# -gt $EXPECTED_ARGS ]
  then
    echo "Usage: `basename $0` <max_number_of_jobs>"
    exit $E_BADARGS
  fi

  number_jobs=$(qstat -u $USERNAME | wc -l)
  while [ $number_jobs -gt $JOBS ]
  do
    echo -e "\e[01;34mQueue limit reached: waiting $TO_SLEEP seconds before next trial\e[00m"
    sleep $TO_SLEEP
    number_jobs=$(qstat -u $USERNAME | wc -l)
  done
}

################################################################################
################################################################################

# Usage:
# sedlauncher 
function sedlauncher {
  EXPECTED_ARGS=9
  E_BADARGS=65
  LAUNCHER_TEMPLATE="launcher.sh"

  if [ $# -gt $EXPECTED_ARGS ]
  then
    echo "Usage: `basename $0` too few parameters in sedlauncher!"
    exit $E_BADARGS
  fi

  # Let's use l_ for local variables!
  l_output=$1
  l_jobcounter=$2
  l_username=$3
  l_export_argos=$4
  l_command=$5
  l_resultsdir=$6
  l_jobdir=$7
  l_queue_type=$8
  l_job_prefix=$9

  if [ "$l_queue_type" == "any" ]
  then
    l_queue_type=""
  fi


  
  sed "{
    s/JOBCOUNTER/$l_jobcounter/g
    s/USERNAME/$l_username/g
    s/EXPORTARGOS/$l_export_argos/g
    s,THECOMMAND,\"$l_command\",g
    s,RESDIR,\"$l_resultsdir\",g
    s,JDIR,\"$l_jobdir\",g
    s,QUEUE_TYPE,$l_queue_type,g
    s,JOBPREFIX,$l_job_prefix,g
  }" $LAUNCHER_TEMPLATE > $l_output
}



	