# Usage: bash start_xyz.sh <node1> <node2> <decision_rule>
TEMPLATE='../experiments/epuck_blockchain_template.argos'
OUTFILE="../experiments/epuck_blockchain_created.argos"
BASEDIR='/home/root/vstrobel/'
BLOCKCHAINPATH="/home/root/vstrobel/eth_data_para/data" # always without '/' at the end!!
NUMROBOTS=(20) 
REPETITIONS=50
DECISIONRULE=1
PERCENT_BLACK=(34)
MININGDIFF=1000000 #was 1000000 before 
# never go with the difficulty below 131072! (see https://github.com/ethereum/go-ethereum/issues/3590)
USEMULTIPLENODES=true
USEBACKGROUNDGETHCALLS=true
MAPPINGPATH="/home/root/vstrobel/Documents/argdavide/experiments/config.txt"
CHANGEDIFFIULTY=""
NUMRUNS=1
THREADS=20
NOW=`date +"%d-%m-%Y"`
BASEPORT=33200
echo "BASEPORT is ${BASEPORT}"
DATADIR="data/experiment1_decision${DECISIONRULE}-${NOW}/"
REGENERATEFILE="$(pwd)/regenerate0.sh"
# The miner node is the first of the used nodes

USECLASSICALAPPROACH=true
# TODO:
#NUMBYZANTINE=(0 1 2 3 4 5 6 7 8 9)
NUMBYZANTINE=(0)
BYZANTINESWARMSTYLE=0
SUBSWARMCONSENSUS=false # Determines if all N robots have to agree or
		       # only the beneficial subswarm.

mkdir -p $DATADIR

PERCENT_WHITE=$(expr 100 - $PERCENT_BLACK)

# Create template
sed -e "s|BASEDIR|$BASEDIR|g" -e "s|NUMRUNS|$NUMRUNS|g" -e "s|DATADIR|$DATADIR|g" -e "s|RADIX|$RADIX|g" -e "s|NUMROBOTS|$k|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_BLACK|$PERCENT_BLACK|g" -e "s|PERCENT_WHITE|$PERCENT_WHITE|g" -e "s|DECISIONRULE|$DECISIONRULE|g" -e "s|USEMULTIPLENODES|$USEMULTIPLENODES|g" -e "s|MININGDIFF|$MININGDIFF|g" -e "s|MINERNODE|$MINERNODE|g" -e "s|MINERID|$MINERID|g" -e "s|BASEPORT|$BASEPORT|g" -e "s|USEBACKGROUNDGETHCALLS|$USEBACKGROUNDGETHCALLS|g" -e "s|BLOCKCHAINPATH|$BLOCKCHAINPATH|g" -e "s|MAPPINGPATH|$MAPPINGPATH|g" -e "s|THREADS|$THREADS|g" -e "s|USECLASSICALAPPROACH|$USECLASSICALAPPROACH|g" -e "s|NUMBYZANTINE|$y|g" -e "s|BYZANTINESWARMSTYLE|$BYZANTINESWARMSTYLE|g" -e "s|SUBSWARMCONSENSUS|$SUBSWARMCONSENSUS|g" -e "s|REGENERATEFILE|$REGENERATEFILE|g" $TEMPLATE > $OUTFILE
