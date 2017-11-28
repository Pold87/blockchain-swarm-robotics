NUMGETHS=4
TEMPLATEA=/home/vstrobel/go-ethereum/circle.yml.template
TEMPLATEB=/home/vstrobel/go-ethereum/vendor/github.com/ethereum/ethash/ethash.go.template
TEMPLATEC=~/go-ethereum/core/block_validator.go.template

if [ $# -eq 0 ]
  then
    MININGDIFF=1000000
else
    MININGDIFF=$1
fi


for i in 8 9 10 13; do

    cp -r "/home/vstrobel/go-ethereum" "/home/vstrobel/go-ethereum${i}"

    OUTFILEA="/home/vstrobel/go-ethereum${i}/circle.yml"

    sed -e "s|MYNUMBER|$i|g" $TEMPLATEA > $OUTFILEA

    OUTFILEB="/home/vstrobel/go-ethereum${i}/vendor/github.com/ethereum/ethash/ethash.go"
    sed -e "s|MYNUMBER|$i|g" $TEMPLATEB > $OUTFILEB

    OUTFILEC="/home/vstrobel/go-ethereum${i}/core/block_validator.go"

    sed -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATEC > $OUTFILEC
    
    make -C "/home/vstrobel/go-ethereum${i}"
    mv "/home/vstrobel/go-ethereum${i}/build/bin/geth" "/home/vstrobel/go-ethereum${i}/build/bin/geth${i}"

    #cp "/home/vstrobel/.ethash/." "/home/vstrobel/.ethash${i}/"
    
done


