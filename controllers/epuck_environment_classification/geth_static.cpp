#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#define DEBUG false

using namespace std;

string datadir_base =  "~/Documents/eth_data/data";

const int rpc_base_port = 8100;
const int ipc_base_port = 31000;
const int maxtrials = 40;

/*
  Convert a robot Id (fbxxx) to an integer (xxx)
*/
uint Id2Int(std::string id) {

  uint idConversion = id[2] - '0';
  if(id[3]!='\0')
    idConversion = (idConversion * 10) + (id[3] - '0');

    return idConversion;
  
}

// Execute command line program and return string result

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


// Takes a geth command, executes it on the selected robot, and returns the result string
string exec_geth_cmd(int i, string command){

  ostringstream fullCommandStream;

  fullCommandStream << "geth --exec " << "'" << command << "'" << " attach " << datadir_base << i << "/" << "geth.ipc";
  
  std::string fullCommand = fullCommandStream.str();

  if (DEBUG)
    cout << "exec_geth_cmd: " << fullCommand << endl;
  
  string res = exec(fullCommand.c_str());

  return res;
  
}


void createAccount(int i) {
    sleep(1);
  std::ostringstream fullCommandStream;
  fullCommandStream << "personal.newAccount(\"test\")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

}


void geth_init(int i) {
  cout << "Calling geth_init for robot " << i << endl;

  std::ostringstream datadirStream;

  datadirStream << "~/Documents/eth_data/data" << i;
  
  string str_datadir = datadirStream.str();
  string genesis = "~/genesis/genesis1.json ";
  
  ostringstream fullCommandStream;

  fullCommandStream << "geth --verbosity 2" << " --datadir " << str_datadir << " init " << genesis;

  string commandStream = fullCommandStream.str();
  
  exec(commandStream.c_str());
  sleep(1);
    
}

string get_enode(int i) {

  ostringstream outfileStream;
  outfileStream << "enode_" << i << ".txt";
  string out_file = outfileStream.str();
  string base_command = "geth --exec 'admin.nodeInfo.enode' attach ";

  std::ostringstream datadirStream;
  datadirStream << "~/Documents/eth_data/data" << i << "/";
  
  string str_datadir = datadirStream.str();
  string interface = str_datadir + "geth.ipc";
  string command = base_command + interface +  " >> " + out_file;
  string command_nopipe = base_command + interface;

  // Run the command and save result in a string
  string res = exec(command_nopipe.c_str());

  return res;
  
}

void start_geth(int i) {
      sleep(1);
  //string base_command = "geth --networkid \"1\" --rpc --ipcdisable --rpcaddr \"0.0.0.0\" --rpccorsdomain \"*\"";

  cout << "Starting geth for robot " << i << endl;
  
  string base_command = "geth --verbosity \"2\" --networkid 2 --nodiscover ";

  std::ostringstream datadirStream;
  datadirStream << "~/Documents/eth_data/data" << i << + "/";
  
  string str_datadir = datadirStream.str();
  string rpc = " --rpc --rpcaddr 0.0.0.0 --rpccorsdomain \"*\" ";
  ostringstream rpcStream, portStream;
  rpcStream << "--rpcport " << (rpc_base_port + i);
 
  string rpcport = rpcStream.str();

  portStream << " --port " << (ipc_base_port + i);
  string str_port = portStream.str();

  // Make folders
  string mk_folders = "mkdir -p " + str_datadir;
  
  system(mk_folders.c_str());

  // Compose geth command

  // Add the IPC interface to the first robot, disable it for the rest
  string ipcdisable = " ";
  // if (i != 0)
  //   ipcdisable = " --ipcdisable ";
  // else
  //   ipcdisable = " ";

  string ipc_path = "--ipcpath " + str_datadir + "geth.ipc";
  //string ipc_path = " ";

  
  string geth_command = base_command + ipcdisable + ipc_path + rpc + rpcport + " --datadir " + str_datadir + str_port + "&";

  cout << "Running command " << geth_command << endl;


  //std::shared_ptr<FILE> pipe(popen(geth_command.c_str(), "r"), pclose);
  //system(geth_command.c_str());
  FILE* pipe = popen(geth_command.c_str(), "r");
  pclose(pipe);
	  
  //exec(geth_command.c_str());
 
}

/* Start the mining process for robot i using t threads */
string start_mining(int i, int t) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "miner.start(" << t << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  return res;

  
}

/* Stop the mining process for robot i */
string stop_mining(int i) {
  string cmd = "miner.stop()";
  string res = exec_geth_cmd(i, cmd);
  return res;
}


string load_contract(int i, string contractName, string contractAddress, string contractInterface){
  std::ostringstream fullCommandStream;

  /* Create contract interface */
  fullCommandStream << "var " << contractName << "Contract = web3.eth.contract(" << contractInterface << ")" << "\n"
    /* Get specific contract at address */
    //		    << "var " << contractName << " = " << contractName << "Contract.at(\"" << contractAddress << "\")";
    ;
  
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  return res;
}


// Add enode to robot i
string add_peer(int i, string enode) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.addPeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  if (DEBUG)
    cout << "add_peer: enode is " << enode << "result is" << res << endl;

  return res;
}

// Remove enode from robot i
string remove_peer(int i, string enode) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.removePeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  return res;
}

/* Reads the first line from a file */
string readStringFromFile(string fileName){
  string s;
  ifstream infile;
  infile.open(fileName.c_str());
  getline(infile, s); // Saves the line in s.
  infile.close();

  return s;
}

/* Reads an entire file */
string readEntireFile(string fileName){
  cout << fileName << endl;
  string s;
  std::ostringstream fullCommandStream;
  ifstream infile;
  infile.open (fileName.c_str());
  while(getline(infile, s)); // Saves the line in STRING.) // To get you all the lines.
    {
      fullCommandStream << s;
      cout << "READ" << s;
    }
  infile.close();

  string cmd = fullCommandStream.str();
  return cmd;
}


// Get coinbase address of robot i
std::string getCoinbase(int i){
  std::ostringstream fullCommandStream;
  fullCommandStream << "eth.coinbase";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

    if (DEBUG)
      cout << "DEBUG  -- getCoinbase " << "(robot " << i << "): " << res << endl;
  
  return res;
}

// Send v ether from robot i to address to
std::string sendEther(int i, std::string from, std::string to, int v) {
  std::ostringstream fullCommandStream;
  fullCommandStream << "eth.sendTransaction({from:"  << from << ",to:" << to << ", value: web3.toWei(" << v << ", \"ether\")})";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  if (DEBUG)
    cout << "DEBUG -- sendEther (from " << i << " to " << to << "): " << res << endl;
    
  return res;
}

// Get contract address from transaction receipt
std::string getContractAddress(int i, std::string txHash) {
  std::ostringstream fullCommandStream;

  cout << "txHash is: " << txHash << endl;
  
  fullCommandStream << "eth.getTransactionReceipt(\"" << txHash << "\").contractAddress";
  
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  if (DEBUG)
    cout << "DEBUG -- getContractAddress: " << res << endl;
    
  if (res.find("undefined") != string::npos) {
    cout << "Contract address not specified! Exiting" << endl;
    exec("killall geth");
    throw;
  }

  return res;
}

/* Unlock account */
std::string unlockAccount(int i, std::string pw) {
    std::ostringstream fullCommandStream;

  fullCommandStream << "personal.unlockAccount(eth.coinbase, \"" << pw << "\", 50000)";
  
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  if (DEBUG)
    cout << "DEBUG -- unlockAccount: " << res << endl;
    
  return res;
}


std::string kill_geth_thread(int i) { 

  int port = rpc_base_port + i;
  
  std::ostringstream fullCommandStream;
  fullCommandStream << "ps ax | grep \"\\-\\-rpcport " << port << "\"";
  string cmd = fullCommandStream.str();
  string res = exec(cmd.c_str());

  if (DEBUG)
    cout << "DEBUG -- kill_geth_thread: " << res << endl;


  /* Only get the first word, i.e., the PID from the command */
  istringstream iss(res);
  std::string pid;

  iss >> pid;

  cmd = "kill " + pid;
  res = exec(cmd.c_str());
  
  if (DEBUG)
    cout << "DEBUG -- kill_geth_thread: " << res << endl;  
  
  return res;
  
}

/* Deploy contract using robot number i and return the transaction hash */
std::string deploy_contract(int i, string contractPath) {
    
  for (int trials = 0; trials < maxtrials; ++trials) {

    if (DEBUG)
      cout << "Trials is: " << trials << endl;

    string txHashRaw = exec_geth_cmd(i, "loadScript(\"" + contractPath + "\")");
    cout << "txHashRaw: " << txHashRaw << endl; 
    string txHash;
    std::istringstream f(txHashRaw);
    std::getline(f, txHash);

    cout << "txHash: " << txHash << endl; 

    /* If a transaction hash was generated, i.e., neither true nor false were found */
    if (txHash.find("true") == string::npos && txHash.find("false") == string::npos) {
	return txHash;
      }
    }  

  /* If the maximum number of trials is reached */
  cout << "Maximum number of trials is reached!" << endl;
  exec("killall geth");
  throw;
}
 
/* Check account balance of robot i (in wei)*/
int check_balance(int i) {
  string cmd = "eth.getBalance(eth.coinbase)";
  string res = exec_geth_cmd(i, cmd);
  
  int balance = atoi(res.c_str());

  return balance;

  
}

// Get blockchain length of robot i
int getBlockChainLength(int i) {
  
  string cmd = "eth.blockNumber";
  string res = exec_geth_cmd(i, cmd);
  
  int blockNumber = atoi(res.c_str());

  return blockNumber;

}

