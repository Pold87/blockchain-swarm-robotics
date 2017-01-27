TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck.argos'
#BASEDIR='/home/volker/Downloads/code/code/argos_simulations/Epuck/controllers/epuck_environment_classification/'
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
DATADIR='verysimilar-rev/'
#NUMROBOTS=(6 10 14 18 22 26 30)
NUMROBOTS=(10)
REPETITIONS=10

#PERCENT_REDS=(52 56 60 64 68 72 76)
#PERCENT_REDS=(36)
PERCENT_REDS=(100)

mkdir $DATADIR

for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)

    for p in "${PERCENT_REDS[@]}"; do

	PERCENT_RED=$p
	PERCENT_BLUE=$(expr 100 - $PERCENT_RED)
	RADIX=$(printf 'volker_num%d_red%d_blue%d' $k $PERCENT_RED $PERCENT_BLUE)
	
	for i in `seq 1 $REPETITIONS`; do
	    
    # Create template
	    sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX$i|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_RED|g" -e "s|PERCENT_BLUE|$PERCENT_BLUE|g" $TEMPLATE > $OUTFILE
	    
    # Start experiment
	    argos3 -c $OUTFILE
	    
	done
	
    done
    
done
