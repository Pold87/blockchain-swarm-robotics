# This script runs all experiments specified in Gabri's work
k=10
R0=$(expr $k / 2)
B0=$(expr $k / 2)
TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck.argos'
REPETITIONS=10
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
DATABASE=data
MININGDIFF=1000000
DEBUG=""

# Rebuild geth with a fixed mining difficulty
./change_difficulty.sh $MININGDIFF

# # Experiment 1: 
# # Compare decision rules; Simple scenario (66 %, corresponds to Fig. 2) and difficult scenario (52 %, corresponds to Fig. 3)
# mkdir "$DATABASE/exp_1"
# PERCENTBLACKS=(66 52)
# for PERCENTBLACK in "${PERCENTBLACKS[@]}"; do
#     echo $PERCENTBLACK
#     PERCENTWHITE=$(expr 100 - $PERCENTBLACK)
#     for DECISIONRULE in {1..3}; do
# 	DATADIR="$DATABASE/exp_1/percentblack${PERCENTBLACK}_decision$DECISIONRULE/"
# 	mkdir -p $DATADIR
# 	echo $DATADIR
# 	for i in `seq 1 $REPETITIONS`; do

# 	    RADIX="$i"
# 	    echo $RADIX
# 	    if [ ! $DEBUG ]
# 	    then
  
# 	# Create template
# 		sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENTWHITE|g" -e "s|PERCENT_BLUE|$PERCENTBLACK|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATE > $OUTFILE
		
# 	    # Start experiment
# 		argos3 -c $OUTFILE
# 	    fi	    
# 	done
#     done
# done


# # Experiment 2: 
# # Increase initial number E_a(0) of robots having opinion black, corresponds to Fig. 4
# # For the simple setup
# mkdir "$DATABASE/exp_2"
# PERCENTBLACKS=(66 52)
# for PERCENTBLACK in "${PERCENTBLACKS[@]}"; do
#     PERCENTWHITE=$(expr 100 - $PERCENTBLACK)
#     for B0 in `seq 1 $NUMROBOTS`; do
# 	for DECISIONRULE in {1..3}; do
# 	    DATADIR="$DATABASE/exp_2/percentblack${PERCENTBLACK}_decision${DECISIONRULE}_Ea${B0}/"
# 	    mkdir -p $DATADIR	
# 	    echo $DATADIR
# 	    for i in `seq 1 $REPETITIONS`; do

# 		RADIX="$i"
# 		echo $RADIX
# 		if [ ! $DEBUG ]
# 		then
		
# 		sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENTWHITE|g" -e "s|PERCENT_BLUE|$PERCENTBLACK|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATE > $OUTFILE

# 		argos3 -c $OUTFILE
# 		fi
# 	    done
# 	done
#     done
# done

# Experiment 3:
# Increase difficulty of the decision-making problem; compare strategies
# Swarm with N = 20 robots and N = 100 robots
mkdir "$DATABASE/exp_3"
PERCENTBLACKS=(52 56 60 64 68 72 76 80 84 88 92 96)
NUMROBOTS=(20 100)
for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)
        
    for PERCENTBLACK in "${PERCENTBLACKS[@]}"; do
	PERCENTWHITE=$(expr 100 - $PERCENTBLACK)
	# TODO: change back to 1..3
	for DECISIONRULE in {2..2}; do
	    
	    DATADIR="$DATABASE/exp_3/N${k}_percentblack${PERCENTBLACK}_decision${DECISIONRULE}/"
	    mkdir -p $DATADIR
	    echo $DATADIR
	    for i in `seq 1 $REPETITIONS`; do

		RADIX="$i"
		echo $RADIX
		if [ ! $DEBUG ]
		then


		sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENTWHITE|g" -e "s|PERCENT_BLUE|$PERCENTBLACK|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATE > $OUTFILE

		argos3 -c $OUTFILE
		fi
	    done
	done
    done
done
