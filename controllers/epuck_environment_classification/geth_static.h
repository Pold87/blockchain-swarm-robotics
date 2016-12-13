#include <string>

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

// Start mining (robot i)
std::string start_mining(int i);

// Stop mining (robot i)
std::string stop_mining(int i);

// Read first line of file fileName and return as string
std::string readStringFromFile(std::string fileName);

// Get coinbase address of robot i
std::string getCoinbase(int i);

// Send ether from robot i to address addr
std::string sendEther(int i, std::string from, std::string to, int v);

// Create an interface to a contract for robot i
std::string load_contract(int i, std::string contractName, std::string contractAddress, std::string contractInterface);

/* Unlock account */
std::string unlockAccount(int i, std::string pw);

// Get contract address from transaction receipt
std::string getContractAddress(int i, std::string txHash);

std::string readEntireFile(std::string fileName);

/* Kill geth thread based on robot number i */
std::string kill_geth_thread(int i);
