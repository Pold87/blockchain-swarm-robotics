TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck.argos'
#BASEDIR='/home/volker/Downloads/code/code/argos_simulations/Epuck/controllers/epuck_environment_classification/'
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
DATADIR='data/debug/'
NUMROBOTS=(10)
REPETITIONS=10
DECISIONRULE=2
PERCENT_BLACKS=(0)

mkdir -p $DATADIR

for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)

    for p in "${PERCENT_BLACKS[@]}"; do

	PERCENT_BLACK=$p
	PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)

	for i in `seq 1 $REPETITIONS`; do

	    RADIX=$(printf 'num%d_black%d%d' $k $PERCENT_BLACK $i)

	# Create template
	sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_BLUE|$PERCENT_BLACK|g" -e "s|PERCENT_RED|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|REPETITIONS|$REPETITIONS|g" $TEMPLATE > $OUTFILE
	
    # Start experiment
	argos3 -c $OUTFILE
	    
	done
	
    done
    
done
