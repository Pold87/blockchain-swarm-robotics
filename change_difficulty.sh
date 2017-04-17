TEMPLATE=~/go-ethereum/core/block_validator.go.template
#TEMPLATE=~/go-ethereum/consensus/ethash/consensus.go.template
#OUTFILE=~/go-ethereum/consensus/ethash/consensus.go
OUTFILE=~/go-ethereum/core/block_validator.go

if [ $# -eq 0 ]
  then
    MININGDIFF=100000
else
    MININGDIFF=$1
fi

sed -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATE > $OUTFILE

# Rebuild geth
make -C ~/go-ethereum



