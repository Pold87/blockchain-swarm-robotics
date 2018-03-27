USEDNODES=( )

mkdir -p /home/iridia/eth_data_para/data


# Create file for killing the blockchain proceeses on these nodes
echo -n "/home/iridia/blockchain-swarm-robotics/killblockchainallccallpara " > "/home/iridia/eth_data_para/data/bckillerccall"
echo -n "/home/iridia/blockchain-swarm-robotics/killblockchainallpara " > "/home/iridia/eth_data_para/data/bckiller"
for u in "${USEDNODES[@]}"; do
    echo -n "$u " >> "/home/iridia/eth_data_para/data/bckillerccall"
    echo -n "$u " >> "/home/iridia/eth_data_para/data/bckiller" 
done
