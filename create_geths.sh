RESERVEDNODES=(0 1)
TEMPLATEA="$HOME/go-ethereum/circle.yml.template"
TEMPLATEB="$HOME/go-ethereum/vendor/github.com/ethereum/ethash/ethash.go.template"
TEMPLATEC="$HOME/go-ethereum/core/block_validator.go.template"

if [ $# -eq 0 ]
  then
    MININGDIFF=1000000
else
    MININGDIFF=$1
fi


for i in $RESERVEDNODES; do

    cp -r "$HOME/go-ethereum/" "$HOME/go-ethereum${i}"

    OUTFILEA="$HOME/go-ethereum${i}/circle.yml"

    sed -e "s|MYNUMBER|$i|g" $TEMPLATEA > $OUTFILEA

    OUTFILEB="$HOME/go-ethereum${i}/vendor/github.com/ethereum/ethash/ethash.go"
    sed -e "s|MYNUMBER|$i|g" $TEMPLATEB > $OUTFILEB

    OUTFILEC="$HOME/go-ethereum${i}/core/block_validator.go"

    sed -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATEC > $OUTFILEC
    
    make -C "$HOME/go-ethereum${i}"
    mv "$HOME/go-ethereum${i}/build/bin/geth" "$HOME/go-ethereum${i}/build/bin/geth${i}"
    
done


