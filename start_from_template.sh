TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck.argos'
#BASEDIR='/home/volker/Downloads/code/code/argos_simulations/Epuck/controllers/epuck_environment_classification/'
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
DATADIR='opiniontest/'
#NUMROBOTS=(6 10 14 18 22 26 30)
NUMROBOTS=(10)
REPETITIONS=10

#PERCENT_REDS=(52 56 60 64 68 72 76)
#PERCENT_REDS=(36)
#PERCENT_BLACKS=(52 56 60 64 68 72 76)
PERCENT_BLACKS=(0)

mkdir $DATADIR

for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)

    for p in "${PERCENT_BLACKS[@]}"; do

	PERCENT_BLACK=$p
	PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)
	RADIX=$(printf 'volker_num%d_black%d_white%d' $k $PERCENT_BLACK $PERCENT_WHITE)
	
	for i in `seq 1 $REPETITIONS`; do
	    
    # Create template
	    sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX$i|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_BLACK|g" -e "s|PERCENT_BLUE|$PERCENT_WHITE|g" $TEMPLATE > $OUTFILE
	    
    # Start experiment
	    argos3 -c $OUTFILE
	    
	done
	
    done
    
done
