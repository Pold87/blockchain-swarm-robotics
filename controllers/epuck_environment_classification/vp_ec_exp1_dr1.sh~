source utils.sh 

###################################################################
# Problem parameters                                              #
###################################################################

SWARMSIZE=( 100 )
length=0   		      # In seconds
randomSeed=$RANDOM

# Parameters shared from <controller> and <loop_function>
g=2     	      # Parameter for the mean of the exponential distribution for the dissemination state
sigma=( 10 )         # Parameter for the mean of the exponential distribution for the exploration state2 4 6 8 10 12 14 16 18 20
	#30 40 50 60 70 80 90 100
	#110 120 130 140 150 160 170 180 190 200
runs=100        	      # Number of runs
decisionRule=( 3 )  # Decision rule
radix=""      	      # Radix for the file names
set=0
# Parameters of <loop_function>
nRedCells=400    	      # For set cell colors without using percentage
nGreenCells=0 		      # For set cell colors without using percentage
nBlueCells=0   		      # For set cell colors without using percentage
usingPercentage=true          # Flag to decide if use or not percentage in cell colors attribution
percentageRed=( 52 66 )        # Percentage of red cells 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50
percentageBlue=( 48 )       # Percentage of blue cells
percentageGreen=0  	      # Percentage of green cells
saveEveryStep=10   	      # Interval of catching statistics
saveEveryTicksStatistic=false  # File with every tick statistics
saveEveryRunStatistics=true  # File with every run statistics
saveEveryRobotStatistics=false  # File with statistics catched by every robot	
saveEveryQualityFile=false    # File where every robot writes his quality after every diffusing stae
globalStatisticsFile=false    # File whit overall statistics
initialReds=( 1 )	      # Number of robots with initial opinion red
initialGreens=( 0 )	      # Number of robots with initial opinion green
initialBlues=( 65 )	      # Number of robots with initial opinion blue
lamda=100                     # Lamda param (Parameter to go straight in the random walk: Exponential distribution mean)
turn=45                       # Turn param (Parameter to turn in the random walk: Uniform distribution mean)
lengthOfOneRun=0
numberOfQualities=10
exitFlag=true		      # Set to TRUE if you want to finish the experiment for consensous reached
numPackSaved=( 2 ) 
lostJobs=( 1 5 16 22 53 25 32 87 47 34 40 41 51 86 64 90 94 ) #99 89 25 88 84 30 82 32 80 34 79 76 38 74 40 72 52 63 70 16 68 47 64 11 21 24 36 68 45 41 47 48 49 50 57 58 60 64 6 39 29 28 )
###################################################################
# Script constants                                                #
###################################################################
USER_NAME=`whoami`
TMP1=`mktemp`
TMP2=`mktemp`
WORKDIR="/tmp/$USER_NAME/"
###################################################################
# launcher.sh configuration constants                             #
##################################################################

# EXPORTARGOS (0 --> TRUE, !=0 --> FALSE)
export_argos=0

# QUEUE_TYPE ("any" --> ANY, coda1 = "#$ -l opteron6272", coda2 = "#$ -l xeon5410, coda3 = "#$ -l opteron6128", coda4 = "#$ -l opteron6272")
#queue_type="#$ -l opteron2216"
#queue_type="#$ -l xeon5410"
#queue_type="#$ -l opteron6128"
#queue_type="#$ -l opteron6272"
queue_type="any"

# DIR_PREFIX
dirprefix="exp-vary-dr3last"
# RESULTSDIR (final destination of the results files of the job)
resultsdir="/lustre/home/$USER_NAME/working/$dirprefix-results/"
# JOBDIR (final destination of the .err and .out files of the job)
jobdir="/lustre/home/$USER_NAME/working/$dirprefix-job_outputs/"
# SCRIPTSDIR (final destination of the script of the job)
scriptsdir="/lustre/home/$USER_NAME/working/$dirprefix-scripts/"
# THECOMMAND
argos_template="/lustre/home/$USER_NAME/svn/Epuck/experiments/c_epuck_environment_classification.argos"
command="/lustre/home/$USER_NAME/argos3-dist/bin/argos3"

# JOBPREFIX
job_prefix="exp-var-dr3last-"
# Counter for the jobs
jobcounter=1

###################################################################
# The actual script                                               #
###################################################################

# Making working directory
mkdir -p $WORKDIR

# Making results directory
mkdir -p $resultsdir

# Making scripts directory
mkdir -p $scriptsdir

# Making job_outputs directory
mkdir -p $jobdir

for ss in "${SWARMSIZE[@]}" ; do
  #for s in "${sigma[@]}" ; do
    for dr in "${decisionRule[@]}" ; do
      for pr in "${percentageRed[@]}" ; do
        for nps in "${numPackSaved[@]}" ; do
         # for pb in "${percentageBlue[@]}" ; do
	 # for ir in "${initialReds[@]}" ; do
	    #for ib in "${initialBlues[@]}" ; do
	    for((s=10;s<=10;s++)); do
   #for((i=1;i<=100;i++)); do
	for((ir=0;ir<=$ss;ir++));do
        #for ir in "${lostJobs[@]}" ; do
            for((k=1;k<=10;k++)); do      # set 
      ###################################################################
      # Configure the script parameters                                 #
      ###################################################################

      # SWARMSIZE
      sed -e "s/SWARM_SIZE/$ss/g" $argos_template > $TMP1

      # MAXTIME
      sed -e "s/LENGTH/$length/g" $TMP1 > $TMP2

      # random seed
      randomSeed=$RANDOM
      sed -e "s/SEED/$randomSeed/g" $TMP2 > $TMP1

      # sigma
      sig=$s
      sed -e "s/SIGMA/$sig/g" $TMP1 > $TMP2

      # g_attribute
      newTry=100
      sed -e "s/PAR_G/100/g" $TMP2 > $TMP1

      # decision rule
      decR=$dr
      sed -e "s/DECISION_RULE/$decR/g" $TMP1 > $TMP2

  
      # Number of red cells if you don't use percentages
      sed -e "s/RED_CELLS/$nRedCells/g" $TMP2 > $TMP1

      # Number of green cells if you don't use percentages
      sed -e "s/GREEN_CELLS/$nGreenCells/g" $TMP1 > $TMP2

      # Number of blue cells if you don't use percentages
      sed -e "s/BLUE_CELLS/$nBlueCells/g" $TMP2 > $TMP1


      # Percentage flag
      sed -e "s/NOT_USING_NUMBER/$usingPercentage/g" $TMP1 > $TMP2

      perRed=$pr
      sed -e "s/P_C_R/$perRed/g" $TMP2 > $TMP1

      # Blue percentage
      perBlue=$((100-$perRed))
      sed -e "s/PERCENT_BLUE/$perBlue/g" $TMP1 > $TMP2

      # Green percentage
      sed -e "s/VERDE_PROPORTION/$percentageGreen/g" $TMP2 > $TMP1

      # Interval of catching stats
      sed -e "s/INTERVAL_CATCH/$saveEveryStep/g" $TMP1 > $TMP2

      # Every ticks flag
      sed -e "s/TICKS_SAVE/$saveEveryTicksStatistic/g" $TMP2 > $TMP1

      # Every run flag
      sed -e "s/EVERY_RUN/$saveEveryRunStatistics/g" $TMP1 > $TMP2

      # Every robot flag
      sed -e "s/WRITEQ/$saveEveryRobotStatistics/g" $TMP2 > $TMP1

      sed -e "s/QUALITY_FILE/$saveEveryQualityFile/g" $TMP1 > $TMP2
      inR=$ir
#      inR=$ir
      sed -e "s/INITIAL_RED_OPINION/$inR/g" $TMP2 > $TMP1

      inB=$(($ss-$inR))
      sed -e "s/INITIAL_BLUE_OPINION/$inB/g" $TMP1 > $TMP2

      sed -e "s/INITIAL_gREEN_OPINION/$initialGreens/g" $TMP2 > $TMP1

      # Lamda value
      sed -e "s/LAMDA/$lamda/g" $TMP1 > $TMP2

      # Turn parameter
      sed -e "s/TURN/$turn/g" $TMP2 > $TMP1

      # Number of runs
      sed -e "s/RUNS/$runs/g" $TMP1 > $TMP2

      # Length of a run
      sed -e "s/R_M_L/$lengthOfOneRun/g" $TMP2 > $TMP1
      
      set=$k

      # RADIX of file names                                                                                                                   
      radix=""
      radix+="N$ss"
      radix+="NRED$inR"
      radix+="NBLUE$inB"
      radix+="NGREEN$initialGreens"
      radix+="G$100"
      radix+="SIGMA$sig"
      radix+="DR$decR"
      radix+="SET$set"
      radix+="PR$perRed"
      radix+="NPS$nps"
      sed -e "s/RADIX/$radix/g" $TMP1 > $TMP2      
      sed -e "s/NUMQUAL/$numberOfQualities/g" $TMP2 > $TMP1
      sed -e "s/ROFLA/$saveOneRobotFlag/g" $TMP1 > $TMP2
      sed -e "s/GLOBALFI/$globalStatisticsFile/g" $TMP2 > $TMP1

      # True -> exit for consensous, False -> exit for number of qualities written
      sed -e "s/EXITFL/$exitFlag/g" $TMP1 > $TMP2
      sed -e "s/NPACKSAV/$nps/g" $TMP2 > $TMP1

      # Set up the list of parameters (only the xml file in this case)
      xml="$scriptsdir"
      xml+="N$ss"
      xml+="NRED$inR"
      xml+="NBLUE$inB"
      xml+="NGREEN$initialGreens"
      xml+="G100"
      xml+="SIGMA$sig"
      xml+="LAMDA$lamda"
      xml+="DR$decR"
      xml+="SET$set"
      xml+="PERCRED$perRed"
      xml+="NPS$nps"
      mv $TMP1 $xml
      parameters="-c $xml"

      ###################################################################
      # Configure the launcher.sh script                                #
      ###################################################################
  
      # Set up launcher script
      launcher="$WORKDIR$jobcounter.sh"
      sedlauncher $launcher $jobcounter $USER_NAME $export_argos "$command" $resultsdir $jobdir "$queue_type" $job_prefix

      ###################################################################
      # Submit the job                                                  #
      ###################################################################
      qwait
      qsub $launcher $parameters
      jobcounter=$(echo "$jobcounter + 1" | bc)
 
      ###################################################################
      # Clean up all the mess                                           #
      ###################################################################
      # Remove tmp files
      rm -f $TMP1 $TMP2 $launcher
       done 
      done
     done
    done
   done
  done
 done
