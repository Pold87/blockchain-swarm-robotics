#include <string>

uint Id2Int(std::string id);

bool replace(std::string& str, const std::string& from, const std::string& to);

std::string replaceAll(std::string subject, const std::string& search,
		  const std::string& replace);

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);

std::string exec_geth_cmd(int i, std::string command);

// Execute a command line command and return the result as string
std::string exec(const char* cmd);

// Initialize geth
void geth_init(int i);

// Get the enode information for robot i
std::string get_enode(int i);

// Run geth for robot i
void start_geth(int i);

/* Create a new account for robot i */
void createAccount(int i);

// Run a geth command on robot i
std::string exec_geth_cmd(int i, std::string command);

// Add a peer (specified via the enode) to robot i
std::string add_peer(int i, std::string enode);

// Remove a peer (specified via the enode) from robot i
std::string remove_peer(int i, std::string enode);

// Start mining (robot i) using t threads
std::string start_mining(int i, int t);

// Stop mining (robot i)
std::string stop_mining(int i);

// Read first line of file fileName and return as string
std::string readStringFromFile(std::string fileName);

// Get coinbase address of robot i
std::string getCoinbase(int i);

// Get blockchain length of robot i
int getBlockChainLength(int i);

// Get number of white votes
int getWhiteVotes(int i);

// Get number of black votes
int getBlackVotes(int i);

// Get last 2 votes
int getLast2Votes(int i);

// Send ether from robot i to address addr
std::string sendEther(int i, std::string from, std::string to, int v);

// Create an interface to a contract for robot i
std::string load_contract(int i, std::string contractName, std::string contractAddress, std::string contractInterface);

// Interact with a generic smart contract
std::string smartContractInterface(int i, std::string interface, std::string contractAddress, std::string func, int args[], int argc, int v);

/* Unlock account */
std::string unlockAccount(int i, std::string pw);

// Get contract address from transaction receipt
std::string getContractAddress(int i, std::string txHash);

std::string readEntireFile(std::string fileName);

/* Kill geth thread based on robot number i */
std::string kill_geth_thread(int i);

/* Deploy contract using robot number i */
std::string deploy_contract(int i, std::string interfacePath, std::string dataPath, std::string templatePath);

/* Check account balance of robot i (in wei)*/
int check_balance(int i);

/* Get the raw transaction based on the tx hash */
std::string getRawTransaction(int i, std::string txHash);

/* Send raw transaction and include it in the tx pool */
std::string sendRawTransaction(int i, std::string rawTx);

/* Find out on which node the geth process of a robot i is executed */
std::string getNode(int i);
int getNodeInt(int i);

/* Convert the hostname of a node to its ip */
std::string hostname2ip(std::string hostname);
