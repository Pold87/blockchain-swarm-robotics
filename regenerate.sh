USEDNODES=( )

mkdir -p /home/vstrobel/eth_data_para/data


# Create file for killing the blockchain proceeses on these nodes
echo -n "/home/vstrobel/Documents/argdavide/killblockchainallccallpara " > "/home/vstrobel/eth_data_para/data/bckillerccall"
echo -n "/home/vstrobel/Documents/argdavide/killblockchainallpara " > "/home/vstrobel/eth_data_para/data/bckiller"
for u in "${USEDNODES[@]}"; do
    echo -n "$u " >> "/home/vstrobel/eth_data_para/data/bckillerccall"
    echo -n "$u " >> "/home/vstrobel/eth_data_para/data/bckiller" 
done
