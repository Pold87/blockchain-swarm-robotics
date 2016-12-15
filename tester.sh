#!/bin/bash


set -e
set -o pipefail

EXECDIR=execdir

QUEUE=long
JOBNAME=gethjob
MACHINE=opteron6128
MPIRUN=/opt/openmpi/bin/mpirun
PARALLEL_ENV=mpich_fu
#PARALLEL_ENV=mpich_rr
NB_PARALLEL_PROCESS=8


exec qsub -v PATH <<EOF &
#!/bin/sh
#$ -N $JOBNAME
#$ -l $MACHINE
#$ -l $QUEUE
# $ -m ase
#$ -o $EXECDIR/gethjob-$$.stdout
#$ -e $EXECDIR/gethjob-$$.stderr
#$ -cwd

source /home/vstrobel/argos3-dist/bin/setup_argos3
argos3 -c /home/vstrobel/Documents/argdavide/experiments/epuck_EC_locale.argos

EOF


