# This script runs all experiments specified in Gabri's work

NUMROBOTS=10
REPETITIIONS=10
DECISIONRULE=3
alias createtemplate="sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX$i|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_BLACK|g" -e "s|PERCENT_BLUE|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" $TEMPLATE > $OUTFILE"


# Experiment 1: 
# Simple scenario, compare decision rules, corresponds to Fig. 2
for DECISIONRULE in {1..3};
do
    DATADIR="simple_decision_$DECISIONRULE"
    PERCENT_BLACKS=66

    for i in `seq 1 $REPETITIONS`; do

    # Create template
    createtemplate

    # Start experiment
    argos3 -c $OUTFILE

    done

done

# Experiment 2:
# Difficult scenario, compare strategies, corresponds to Fig. 3
for DECISIONRULE in {1..3};
do
    DATADIR="difficult_decision_$DECISIONRULE"
    PERCENT_BLACKS=52

    for i in `seq 1 $REPETITIONS`; do

    # Create template
    createtemplate

    argos3 -c $OUTFILE

    done

done 

# Experiment 3: 
# Increase initial number E_a(0), corresponds to Fig. 4
# For the simple setup
PERCENT_BLACKS=66
DATADIR="simple_"
for B0 in `seq 0 $NUMROBOTS`; do
    for i in `seq 1 $REPETITIONS`; do

	createtemplate
	argos3 -c $OUTFILE
    done

done

# For the difficult setup
PERCENT_BLACKS=52
for B0 in `seq 0 $NUMROBOTS`; do
    for i in `seq 1 $REPETITIONS`; do

	createtemplate
	argos3 -c $OUTFILE
    done
done

# Experiment 4:
# Increase difficulty of the decision-making problem; compare strategies
# Swarm with N = 20 robots
NUMROBOTS=20
PERCENT_BLACKS=(52 56 60 64 68 72 76 80 84 88 92 96)
for PERCENT_BLACK in "${PERCENT_BLACKS[@]}"; do

    for i in `seq 1 $REPETITIONS`; do

	createtemplate
	argos3 - $OUTFILE

    done

done


# Swarm with N = 100 robots
NUMROBOTS=100