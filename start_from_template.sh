TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck.argos'
#BASEDIR='/home/volker/Downloads/code/code/argos_simulations/Epuck/controllers/epuck_environment_classification/'
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
DATADIR='data/experiment1_decisionrule3/'
NUMROBOTS=(20)
REPETITIONS=15
DECISIONRULE=3
PERCENT_BLACKS=(48)
# the one I did all the tests with:
MININGDIFF=1000000
##MININGDIFF=150000
# never go with the difficulty below 131072! (see https://github.com/ethereum/go-ethereum/issues/3590)
#MININGDIFF=
USEMULTIPLENODES=true
CHANGEDIFFIULTY=""

mkdir -p $DATADIR

if [ $CHANGEDIFFIULTY ]; then
    #    ./change_difficulty.sh $MININGDIFF
    ./create_geths.sh $MININGDIFF
fi


for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)

    for p in "${PERCENT_BLACKS[@]}"; do

	PERCENT_BLACK=$p
	PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)

	for i in `seq 1 $REPETITIONS`; do

	    GENERATEDAG=`cat regeneratedag.txt`
	    if [ $GENERATEDAG ]; then
		#if [ "$i" -gt 0 ]; then
		rm ~/.ethash/*
		ssh vstrobel@c3-5 "geth makedag 0 ~/.ethash"
		echo "" > regeneratedag.txt
		#fi
	    fi

	    RADIX=$(printf 'num%d_black%d%d' $k $PERCENT_BLACK $i)

	# Create template
	sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_BLUE|$PERCENT_BLACK|g" -e "s|PERCENT_RED|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|USEMULTIPLENODES|$USEMULTIPLENODES|g" -e "s|REPETITIONS|$REPETITIONS|g" -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATE > $OUTFILE
	
    # Start experiment
	argos3 -c $OUTFILE
	    
	done
	
    done
    
done
