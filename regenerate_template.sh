USEDNODES=(NODEA NODEB)

mkdir -p BLOCKCHAINPATH


# Create file for killing the blockchain proceeses on these nodes
echo -n "PWD/killblockchainallccallpara " > "BLOCKCHAINPATH/bckillerccall"
echo -n "PWD/killblockchainallpara " > "BLOCKCHAINPATH/bckiller"
for u in "${USEDNODES[@]}"; do
    echo -n "$u " >> "BLOCKCHAINPATH/bckillerccall"
    echo -n "$u " >> "BLOCKCHAINPATH/bckiller" 
done
