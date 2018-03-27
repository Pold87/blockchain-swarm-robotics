# Usage: bash start_xyz.sh <node1> <node2> <decision_rule>
TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE="experiments/epuck$1.argos"
#BASEDIR='/home/volker/Documents/bc_collective/controllers/epuck_environment_classification/'
BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
BLOCKCHAINPATH="/home/vstrobel/eth_data_para$1/data" # always without '/' at the end!!
MINERID=$(expr 120 + $1)
echo "MINERID is ${MINERID}"
NUMROBOTS=(20) 
#REPETITIONS=50
DECISIONRULE=$3
PERCENT_BLACKS=(34 36 38 40 42 44 46 48)
# the one I did all the tests with:
MININGDIFF=1000000
# never go with the difficulty below 131072! (see https://github.com/ethereum/go-ethereum/issues/3590)
USEMULTIPLENODES=true
USEBACKGROUNDGETHCALLS=true
MAPPINGPATH="/home/vstrobel/Documents/argdavide/experiments/config$1.txt"
CHANGEDIFFIULTY=""
NUMRUNS=30

USEDNODES=($1 $2)
echo "USEDNODES is ${USEDNODES}"
BASEPORT=$((31000 + $1 * 200))
echo "BASEPORT is ${BASEPORT}"
DATADIR="data/experiment1_decision${DECISIONRULE}-node$1-newreset/"

# The miner node is the first of the used nodes
MINERNODE=${USEDNODES[0]}

USECLASSICALAPPROACH=false
# TODO:
NUMBYZANTINE=(0)
BYZANTINESWARMSTYLE=1
SUBSWARMCONSENSUS=true # Determines if all N robots have to agree or
# only the beneficial subswarm.

REGENERATEFILE="regenerate${USEDNODES[0]}.sh"

sed -e "s|NODEA|${USEDNODES[0]}|g" -e "s|NODEB|${USEDNODES[1]}|g" -e "s|BLOCKCHAINPATH|$BLOCKCHAINPATH|g" -e "s|PWD|$(pwd)|g" regenerate_template.sh > $REGENERATEFILE
