TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck2.argos'
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
BLOCKCHAINPATH='/home/vstrobel/eth_data_para2/data' # always without '/' at the end!!
MINERID=121
NUMROBOTS=(20) 
REPETITIONS=15
DECISIONRULE=2
PERCENT_BLACKS=(34 48 36 38 40 42 44 46)
# the one I did all the tests with:
MININGDIFF=1000000
# never go with the difficulty below 131072! (see https://github.com/ethereum/go-ethereum/issues/3590)
USEMULTIPLENODES=true
MAPPINGPATH='/home/vstrobel/Documents/argdavide/experiments/config2.txt'
CHANGEDIFFIULTY=""
USEDNODES=(2 3) # TODO: CHANGE BACK !!!
BASEPORT=35000
DATADIR="data/experiment1_decision${DECISIONRULE}/"

 # Rebuild geth with another value in checkDifficulty
 if [ $CHANGEDIFFIULTY ]; then
     #    ./change_difficulty.sh $MININGDIFF
     ./create_geths.sh $MININGDIFF
 fi

 mkdir -p $DATADIR
 
# Iterate over experimental settings and start experiments
for k in "${NUMROBOTS[@]}"; do

    R0=$(expr $k / 2)
    B0=$(expr $k / 2)

    for p in "${PERCENT_BLACKS[@]}"; do

	PERCENT_BLACK=$p
	PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)

	for i in `seq 1 $REPETITIONS`; do

	    # The miner node is the first of the used nodes
	    MINERNODE=${USEDNODES[0]}

	    GENERATEDAG=`cat regeneratedag.txt`
	    if [ $GENERATEDAG ]; then
		#if [ "$i" -gt 0 ]; then
		rm ~/.ethash/*
		ssh vstrobel@c3-5 "geth makedag 0 ~/.ethash"
		echo "" > regeneratedag.txt
		#fi
	    fi

	    # Create the mapping file
	    python experiments/create_node_mapping_call.py $MAPPINGPATH $NUMROBOTS ${USEDNODES[0]} ${USEDNODES[1]}

	    # Create directories for collecting data and the geth processes
	    mkdir -p $BLOCKCHAINPATH
	    
	    # Create file for killing the blockchain proceeses on these nodes
	    echo -n "$(pwd)/killblockchainallccallpara " > "${BLOCKCHAINPATH}/bckillerccall"
	    echo -n "$(pwd)/killblockchainallpara " > "${BLOCKCHAINPATH}/bckiller"
	    for u in "${USEDNODES[@]}"; do
		echo -n "$u " >> "${BLOCKCHAINPATH}/bckillerccall"
		echo -n "$u " >> "${BLOCKCHAINPATH}/bckiller" 
	    done
	    

	    RADIX=$(printf 'num%d_black%d%d' $k $PERCENT_BLACK $i)

	# Create template
	sed -e "s|BASEDIR|$BASEDIR|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_BLUE|$PERCENT_BLACK|g" -e "s|PERCENT_RED|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|USEMULTIPLENODES|$USEMULTIPLENODES|g" -e "s|REPETITIONS|$REPETITIONS|g" -e "s|MININGDIFF|$MININGDIFF|g" -e "s|MINERNODE|$MINERNODE|g" -e "s|MINERID|$MINERID|g" -e "s|BASEPORT|$BASEPORT|g" -e "s|BLOCKCHAINPATH|$BLOCKCHAINPATH|g" -e "s|MAPPINGPATH|$MAPPINGPATH|g" $TEMPLATE > $OUTFILE
	
    # Start experiment
	argos3 -c $OUTFILE

	# Clean up
	bash "${BLOCKCHAINPATH}/bckillerccall"
	mkdir -p "${DATADIR}${p}-${i}"
	mv "${BLOCKCHAINPATH}"* "${DATADIR}${p}-${i}"
	    
	done
	
    done
    
done
