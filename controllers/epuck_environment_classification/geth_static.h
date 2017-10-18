#ifndef GETH_STATIC_H
#define GETH_STATIC_H

#include <string>
#include <vector>

struct blockWithHash {
  int blockNumber;
  std::string hash;
};

template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> split(const std::string &s, char delim);

double get_wall_time();

uint Id2Int(std::string id);

bool replace(std::string& str, const std::string& from, const std::string& to);

std::string replaceAll(std::string subject, const std::string& search,
		  const std::string& replace);

std::string removeSpace(std::string str);

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);

// Read first line of file fileName and return as string
std::string readStringFromFile(std::string fileName);

// Execute a command line command and return the result as string
std::string exec(const char* cmd);

// Take a geth command, execute it on the selected robot, and return the result string
std::string exec_geth_cmd(int i, std::string command);
std::string exec_geth_cmd(int i, std::string command, int nodeInt, std::string datadirBase);

std::string exec_geth_cmd_helper(int i, std::string command, int nodeInt, std::string datadirBase);
std::string exec_geth_cmd_with_geth_restart(int i, std::string command, int nodeInt, int basePort, std::string datadirBase);


void exec_geth_cmd_background(int i, std::string command, int nodeInt, std::string datadirBase);

// Send error messages per email
void sendMail(std::string body);

// Initialize geth
void geth_init(int i);
void geth_init(int i, int nodeInt, int basePort, std::string datadirBase, std::string genesisPath);

// Run geth for robot i
void start_geth(int i);
void start_geth(int i, int nodeInt, int basePort, std::string datadirBase);

/* Create a new account for robot i */
void createAccount(int i);
void createAccount(int i, int nodeInt, int basePort, std::string datadirBase);


// Get the enode information for robot i
std::string get_enode(int i);
std::string get_enode(int i, int nodeInt, int basePort, std::string datadirBase);

// Start mining (robot i) using t threads
std::string start_mining(int i, int t);
std::string start_mining(int i, int t, int nodeInt, std::string datadirBase);
void start_mining_bg(int i, int t, int nodeInt, std::string datadirBase);

// Stop mining (robot i)
std::string stop_mining(int i);
std::string stop_mining(int i, int nodeInt, std::string datadirBase);
void stop_mining_bg(int i, int nodeInt, std::string datadirBase);

// Add a peer (specified via the enode) to robot i
std::string add_peer(int i, std::string enode);
std::string add_peer(int i, std::string enode, int nodeInt, int basePort, std::string datadirBase);
void add_peer_bg(int i, std::string enode, int nodeInt, std::string datadirBase);

// Remove a peer (specified via the enode) from robot i
std::string remove_peer(int i, std::string enode);
std::string remove_peer(int i, std::string enode, int nodeInt, std::string datadirBase);
void remove_peer_bg(int i, std::string enode, int nodeInt, std::string datadirBase);

// Get coinbase address of robot i
std::string getCoinbase(int i);
std::string getCoinbase(int i, int nodeInt, int basePort, std::string datadirBase);

// Get blockchain length of robot i
int getBlockChainLength(int i);
int getBlockChainLength(int i, int nodeInt, std::string datadirBase);

// Get number of white votes
int getWhiteVotes(int i);
int getWhiteVotes(int i, int nodeInt, std::string datadirBase);

// Get number of black votes
int getBlackVotes(int i);
int getBlackVotes(int i, int nodeInt, std::string datadirBase);

// Get last 2 votes
int getLast2Votes(int i);
int getLast2Votes(int i, int nodeInt, std::string datadirBase);

// Send ether from robot i to address addr
std::string sendEther(int i, std::string from, std::string to, int v);
std::string sendEther(int i, std::string from, std::string to, int v, int nodeInt, std::string datadirBase);

// Interact with a generic smart contract
std::string smartContractInterface(int i, std::string interface, std::string contractAddress, std::string func, int args[], int argc, int v);
std::string smartContractInterface(int i, std::string interface, std::string contractAddress, std::string func, int args[], int argc, int v, int nodeInt, std::string datadirBase);
std::string smartContractInterfaceCall(int i, std::string interface, std::string contractAddress, std::string func, int args[], int argc, int v, int nodeInt, std::string datadirBase);
std::string smartContractInterfaceStringCall(int i, std::string interface, std::string contractAddress, std::string func, std::string args[], int argc, int v, int nodeInt, std::string datadirBase);
void smartContractInterfaceBg(int i, std::string interface, std::string contractAddress, std::string func, int args[], int argc, int v, int nodeInt, std::string datadirBase);
void smartContractInterfaceStringBg(int i, std::string interface, std::string contractAddress, std::string func, std::string args[], int argc, int v, int nodeInt, std::string datadirBase);

/* Unlock account */
std::string unlockAccount(int i, std::string pw);
std::string unlockAccount(int i, std::string pw, int nodeInt, int basePort, std::string datadirBase);

// Get contract address from transaction receipt
std::string getContractAddress(int i, std::string txHash);
std::string getContractAddress(int i, std::string txHash, int nodeInt, std::string datadirBase);

/* Kill geth thread based on robot number i */
std::string kill_geth_thread(int i);
void kill_geth_thread(int i, int basePort, int nodeInt, std::string datadirBase);

/* Deploy contract using robot number i */
std::string deploy_contract(int i, std::string interfacePath, std::string dataPath, std::string templatePath);
std::string deploy_contract(int i, std::string interfacePath, std::string dataPath, std::string templatePath, int nodeInt, std::string datadirBase);

/* Check account balance of robot i (in wei)*/
int check_balance(int i);
long long check_balance(int i, int nodeInt, std::string datadirBase);

/* Get the raw transaction based on the tx hash */
std::string getRawTransaction(int i, std::string txHash);
std::string getRawTransaction(int i, std::string txHash, int nodeInt, std::string datadirBase);

/* Send raw transaction and include it in the tx pool */
std::string sendRawTransaction(int i, std::string rawTx);
std::string sendRawTransaction(int i, std::string rawTx, int nodeInt, std::string datadirBase);

/* Find out on which node the geth process of a robot i is executed */
std::string getNode(int i);
int getNodeInt(int i);

/* Convert the hostname of a node to its ip */
std::string hostname2ip(std::string hostname);

/* Measure time and print it */
double measure_time(double ref_time, std::string part_name);

void generate_genesis(std::string address, int basePort);
void prepare_for_new_genesis(int i, int nodeInt, int basePort, std::string blockchainPath);

#endif
