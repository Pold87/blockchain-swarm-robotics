# This script runs all experiments specified in Gabri's work
k=10
R0=$(expr $k / 2)
B0=$(expr $k / 2)

REPETITIIONS=10
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
DATABASE=data

# Experiment 1: 
# Compare decision rules; Simple scenario (66 %, corresponds to Fig. 2) and difficult scenario (52 %, corresponds to Fig. 3)
PERCENT_BLACKS=(66 52)
for PERCENT_BLACK in {PERCENT_BLACKS[@]}; do
    PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)
    for DECISIONRULE in {1..3}; do
	DATADIR="$DATABASE/exp_1_diff_$PERCENT_BLACK_decision_$DECISIONRULE"

	for i in `seq 1 $REPETITIONS`; do

	    RADIX="$i"
	    
	# Create template
	    sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_BLACK|g" -e "s|PERCENT_BLUE|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" $TEMPLATE > $OUTFILE
	
	    # Start experiment
	    argos3 -c $OUTFILE
	    
	done
    done
done


# Experiment 2: 
# Increase initial number E_a(0) of robots having opinion black, corresponds to Fig. 4
# For the simple setup
PERCENT_BLACKS=(66 52)
for PERCENT_BLACK in {PERCENT_BLACKS[@]}; do
    PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)
    for B0 in `seq 0 $NUMROBOTS`; do
	for DECISIONRULE in {1..3}; do
	    DATADIR="$DATABASE/exp_2_diff_$PERCENT_BLACK_decision_$DECISIONRULE_Ea_$B0"
	
	    for i in `seq 1 $REPETITIONS`; do

		RADIX="$i"
		
		sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_BLACK|g" -e "s|PERCENT_BLUE|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" $TEMPLATE > $OUTFILE
		argos3 -c $OUTFILE
	    done
	done
    done
done

# Experiment 3:
# Increase difficulty of the decision-making problem; compare strategies
# Swarm with N = 20 robots and N = 100 robots
PERCENT_BLACKS=(52 56 60 64 68 72 76 80 84 88 92 96)
NUMROBOTS=(20 100)
for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)
        
    for PERCENT_BLACK in "${PERCENT_BLACKS[@]}"; do
	PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)
	for DECISIONRULE in {1..3}; do
	    
	    DATADIR="$DATABASE/exp_3_N$k_Percent$PERCENT_BLACK_decision$DECISIONRULE"
	
	    for i in `seq 1 $REPETITIONS`; do

		RADIX="$i"
	    
		sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_BLACK|g" -e "s|PERCENT_BLUE|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" $TEMPLATE > $OUTFILE
		argos3 - $OUTFILE
		
	    done
	done
    done
done
