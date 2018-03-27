BASE=`pwd`
git clone https://github.com/ethereum/go-ethereum.git
cd go-ethereum
git checkout v1.5.9
git checkout -b fixeddiff

mv core/block_validator.go core/block_validator.go.template
mv circle.yml circle.yml.template
mv vendor/github.com/ethereum/ethash/ethash.go vendor/github.com/ethereum/ethash/ethash.go.template

sed -i '267s/.*/return big.NewInt(MININGDIFF)/' core/block_validator.go.template
sed -i '269s/.*/return big.NewInt(MININGDIFF)/' core/block_validator.go.template

sed -i '7s/ethash/ethashMYNUMBER/' circle.yml.template
sed -i '70s/ethash/ethashMYNUMBER/' vendor/github.com/ethereum/ethash/ethash.go.template

RESERVEDNODES=(0)

TEMPLATEA="$BASE/go-ethereum/circle.yml.template"
TEMPLATEB="$BASE/go-ethereum/vendor/github.com/ethereum/ethash/ethash.go.template"
TEMPLATEC="$BASE/go-ethereum/core/block_validator.go.template"

if [ $# -eq 0 ]
  then
    MININGDIFF=1000000
else
    MININGDIFF=$1
fi


for i in $RESERVEDNODES; do

    cp -r "$BASE/go-ethereum/" "$BASE/go-ethereum${i}"

    OUTFILEA="$BASE/go-ethereum${i}/circle.yml"

    sed -e "s|MYNUMBER|$i|g" $TEMPLATEA > $OUTFILEA

    OUTFILEB="$BASE/go-ethereum${i}/vendor/github.com/ethereum/ethash/ethash.go"
    sed -e "s|MYNUMBER|$i|g" $TEMPLATEB > $OUTFILEB

    OUTFILEC="$BASE/go-ethereum${i}/core/block_validator.go"

    sed -e "s|MININGDIFF|$MININGDIFF|g" $TEMPLATEC > $OUTFILEC
    
    make -C "$BASE/go-ethereum${i}"
    mv "$BASE/go-ethereum${i}/build/bin/geth" "$BASE/go-ethereum${i}/build/bin/geth${i}"
    
done


