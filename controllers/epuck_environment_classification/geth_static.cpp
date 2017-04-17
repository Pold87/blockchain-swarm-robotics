#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#define DEBUG true
#define USE_MULTIPLE_NODES true

using namespace std;

string datadir_base =  "~/Documents/eth_data/data";

const int rpc_base_port = 8300;
const int ipc_base_port = 33500;
const int maxtrials = 3000;

/*
  Convert a robot Id (fbxxx) to an integer (xxx)
*/
uint Id2Int(std::string id) {

  uint idConversion = id[2] - '0';
  if(id[3]!='\0')
    idConversion = (idConversion * 10) + (id[3] - '0');

    return idConversion;
}


/* Replace the pattern from with to in the string str  */
bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

/* Replace all occurrences of search with replace and return new string */

string replaceAll(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}


void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}


/* Execute command line program and return string result */
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


int getNodeInt(int i) {

  int res;

  if (i < 5) {
    res = 1;
  } else if (i < 10) {
    res = 2;
  } else if (i < 15) {
    res = 3;
  } else if (i < 20) {
    res = 4;
  } else if (i < 25) {
    res = 5;
  } else if (i < 86) {
    res = 5;
  } else {
    res = 5;
  }

  return res;  
}

/* Find out on which node the geth process of a robot i is executed */
std::string getNode(int i) {

  int res = getNodeInt(i);

  std::ostringstream ss;
  ss << "c3-" << res;
  return ss.str();  
}

/* Convert the hostname of a node to its ip */
std::string hostname2ip(std::string hostname) {

  ostringstream fullCommandStream;

  fullCommandStream << "host " << hostname << " | awk \'FNR>1 {printf $4}\'";
  
  std::string fullCommand = fullCommandStream.str();
  
  string res = exec(fullCommand.c_str());

  if (DEBUG)
    cout << "The resolved ip address of hostname " << hostname << " is " << res << endl;
    
  return res;
}


// Take a geth command, execute it on the selected robot, and return the result string
string exec_geth_cmd(int i, string command){

  ostringstream fullCommandStream;

  if (USE_MULTIPLE_NODES) {
    /* Find out nodes of this robot  */
    string node = getNode(i);
    int nodeInt = getNodeInt(i);
    /* Run geth command on this node  */
    fullCommandStream << "ssh vstrobel@" << node << " \"";

    ReplaceStringInPlace(command, "\"", "\\\"");

    fullCommandStream << "geth" << nodeInt << " --exec " << "'" << command << "'" << " attach " << datadir_base << i << "/" << "geth.ipc\"";
  } else {

    fullCommandStream << "geth --exec " << "'" << command << "'" << " attach " << datadir_base << i << "/" << "geth.ipc";

  }

  std::string fullCommand = fullCommandStream.str();

  //  if (DEBUG)
  //cout << "exec_geth_cmd: " << fullCommand << endl;
  
  string res = exec(fullCommand.c_str());

  cout << "res in exec_geth_cmd is " << res << endl; 
  
  //cout << "Result of exec_geth_cmd: " << res << endl;
  if ( (res.find("Fatal") != string::npos) || (res.find("Error") != string::npos)) {
    cout << "exec_geth_cmd: " << fullCommand << endl;
    cout << "Result of exec_geth_cmd: " << res << endl;
  }

  /* If a transaction hash was generated, i.e., neither true nor false nor Error were found */
    if (res.find("Fatal") != string::npos) {
      cout << "Fatal error!!!" << endl;
      exec("bash killblockchainallccall");
      throw;
      }
  
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


  if (USE_MULTIPLE_NODES) {
    /* Find out nodes of this robot  */
    string node = getNode(i);
    int nodeInt = getNodeInt(i);
    /* Run geth command on this node  */
    fullCommandStream << "ssh vstrobel@" << node << " \"";

    fullCommandStream << "geth" << nodeInt << " --verbosity 3" << " --datadir " << str_datadir << " init " << genesis << "\"";

  } else {
    fullCommandStream << "geth --verbosity 3" << " --datadir " << str_datadir << " init " << genesis;
  }
    

  string commandStream = fullCommandStream.str();

  if (DEBUG)
    cout << "geth init: " << commandStream << endl; 
  
  exec(commandStream.c_str());
  sleep(1);
    
}

string get_enode(int i) {

  string cmd = "admin.nodeInfo.enode";
  string res = exec_geth_cmd(i, cmd);

  // Print the received enode
  cout << "The enode is " << res << endl;
  
  /* Find out on which cluster node this robot's geth process should be executed */
  string node = getNode(i);
  /* Resolve the hostname to its ip */
  string ip = hostname2ip(node);
  /* Replace [::] (localhost) with actual ip address */
  replace(res, "[::]", ip);
  
  // Print the replaced enode address
  cout << "The enode with the actual ip address is " << res << endl;
  
  return res;
  
}



// string get_enode(int i) {

//   ostringstream outfileStream;
//   outfileStream << "enode_" << i << ".txt";
//   string out_file = outfileStream.str();
//   string base_command = "geth --exec 'admin.nodeInfo.enode' attach ";

//   ostringstream fullCommandStream;

//   if (USE_MULTIPLE_NODES) {
//     /* Find out nodes of this robot  */
//     string node = getNode(i);
//     /* Run geth command on this node  */
//     fullCommandStream << "ssh vstrobel@" << node << " ";
//   }
  
//   std::ostringstream datadirStream;
//   datadirStream << "~/Documents/eth_data/data" << i << "/";
  
//   string str_datadir = datadirStream.str();
//   string interface = str_datadir + "geth.ipc";
//   string command = base_command + interface +  " >> " + out_file;
//   string command_nopipe = base_command + interface;

//   fullCommandStream << command_nopipe;

//   string commandStream = fullCommandStream.str();

//   // Run the command and save result in a string
//   string res = exec(commandStream.c_str());

//   // Print the received enode
//   cout << "The enode is " << res << endl;

//   /* Find out on which cluster node this robot's geth process should be executed */
//   string node = getNode(i);
//   /* Resolve the hostname to its ip */
//   string ip = hostname2ip(node);
//   /* Replace [::] (localhost) with actual ip address */
//   replace(res, "[::]", ip);

//   // Print the replaced enode address
//   cout << "The enode with the actual ip address is " << res << endl;
  
//   return res;
  
// }

void start_geth(int i) {
      sleep(1);
  //string base_command = "geth --networkid \"1\" --rpc --ipcdisable --rpcaddr \"0.0.0.0\" --rpccorsdomain \"*\"";

  cout << "Starting geth for robot " << i << endl;

  ostringstream fullCommandStream;
  
  if (USE_MULTIPLE_NODES) {
    /* Find out nodes of this robot  */
    string node = getNode(i);
    int nodeInt = getNodeInt(i);    
    /* Run geth command on this node  */
    fullCommandStream << "ssh vstrobel@" << node << " \"";
    fullCommandStream << "geth" << nodeInt << " --verbosity 3 --networkid 2 --nodiscover ";
  } else {
    fullCommandStream << "geth --verbosity 3 --networkid 2 --nodiscover ";
  }
  
  std::ostringstream datadirStream;
  datadirStream << "~/Documents/eth_data/data" << i << + "/";
  
  string str_datadir = datadirStream.str();
  ostringstream portStream;

  portStream << " --port " << (ipc_base_port + i);
  string str_port = portStream.str();

  // Make folders
  string mk_folders = "mkdir -p " + str_datadir;
  
  system(mk_folders.c_str());

  // Compose geth command

  string ipc_path = "--ipcpath " + str_datadir + "geth.ipc";

  fullCommandStream << ipc_path << " --datadir " << str_datadir << str_port << " --maxpeers 25";

  //if (USE_MULTIPLE_NODES)
  //  ReplaceStringInPlace(geth_command, "\"", "\\\"");
  
  //fullCommandStream << geth_command;  

  if (USE_MULTIPLE_NODES)
    fullCommandStream << "\"";

  fullCommandStream << "&";
  
  cout << "Running command " << fullCommandStream.str() << endl;
  
  FILE* pipe = popen(fullCommandStream.str().c_str(), "r");
  pclose(pipe);
	  
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
  fullCommandStream << "var " << contractName << "Contract = web3.eth.contract(" << contractInterface << ")" << "\n";
  
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
    if (USE_MULTIPLE_NODES) {
      exec("bash killallgeths");
    } else { 
      exec("killall geth");
    }
    throw;
  }

  return res;
}

/* Unlock account */
std::string unlockAccount(int i, std::string pw) {
  std::ostringstream fullCommandStream;

  fullCommandStream << "personal.unlockAccount(eth.coinbase, \"" << pw << "\", 0)";
  
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  if (DEBUG)
    cout << "DEBUG -- unlockAccount: " << res << endl;
    
  return res;
}


std::string kill_geth_thread(int i) { 

  int port = ipc_base_port + i;

  std::ostringstream fullCommandStream;

  string node;
  if (USE_MULTIPLE_NODES) {
    /* Find out nodes of this robot  */
    node = getNode(i);
    /* Run geth command on this node  */
    fullCommandStream << "ssh vstrobel@" << node << " \"";
    fullCommandStream << "ps ax | grep \\\"\\-\\-ipcport " << port << "\\\"";
    fullCommandStream << "\"";
    
  } else {
  fullCommandStream << "ps ax | grep \"\\-\\-ipcport " << port << "\"";
  }

  string cmd = fullCommandStream.str();
  string res = exec(cmd.c_str());

  
  if (DEBUG)
    cout << "DEBUG -- kill_geth_thread -- command:" << cmd << " result: " << res << endl;


  /* Only get the first word, i.e., the PID from the command */
  istringstream iss(res);
  std::string pid;

  iss >> pid;

  string cmd2;
  if (USE_MULTIPLE_NODES) {
    std::ostringstream fullCommandStream2;
    fullCommandStream2 << "ssh vstrobel@" << node << " ";
    fullCommandStream2 << "kill -HUP " << pid;
    cmd2 = fullCommandStream2.str();
  } else {
  cmd2 = "kill -HUP " + pid;
  }

  res = exec(cmd2.c_str());
  
  if (DEBUG)
    cout << "DEBUG -- kill_geth_thread: " << res << endl;  
  
  return res;
  
}

// Interact with a function of a smart contract
// v: Amount of wei to send
std::string smartContractInterface(int i, string interface, string contractAddress,
				   string func, int args[], int argc, int v) {
  
  
   ostringstream fullCommandStream;

   fullCommandStream << "var cC = web3.eth.contract(" << interface << ");var c = cC.at(" << contractAddress << ");c." << func << "(";


   for(int k = 0; k < argc; k++) {
     fullCommandStream << args[k] << ",";  
   }

   fullCommandStream << "{" << "value: " << v << ", from: eth.coinbase, gas: '1000000'});";
   
  
   std::string fullCommand = fullCommandStream.str();

   //cout << fullCommand << std::endl;


   string res = exec_geth_cmd(i, fullCommand);
   cout << "Result received from SC is: " << res << endl;


   return res;


 }

/* Deploy contract using robot number i and return the transaction hash */
std::string deploy_contract(int i, string interfacePath, string dataPath, string templatePath) {

  // Get smart contract interface
  string interface = readStringFromFile(interfacePath);
  string data = readStringFromFile(dataPath);
  data = "0x" + data; // Make data hexadecimal
  string contractTemplate = readStringFromFile(templatePath);

  replace(contractTemplate, "INTERFACE", interface);
  replace(contractTemplate, "DATA", data);

  // TODO: make this a parameter
  string tmpPath = "/home/vstrobel/Documents/argdavide/tmp.txt";

  std::ofstream out(tmpPath.c_str());
  out << contractTemplate;
  out.close();

  cout << contractTemplate << std::endl;

  for (int trials = 0; trials < maxtrials; ++trials) {

    if (DEBUG)
      cout << "Trials is: " << trials << endl;

    string txHashRaw = exec_geth_cmd(i, "loadScript(\"" + tmpPath + "\")");
    cout << "txHashRaw: " << txHashRaw << endl; 
    string txHash;
    std::istringstream f(txHashRaw);
    std::getline(f, txHash);

    cout << "txHash: " << txHash << endl; 

    /* If a transaction hash was generated, i.e., neither true nor false nor Error were found */
    if (txHash.find("true") == string::npos && txHash.find("false") == string::npos && txHash.find("Error") == string::npos && txHash.find("Fatal") == string::npos) {
	return txHash;
      }
    }  

  /* If the maximum number of trials is reached */
  cout << "Maximum number of trials is reached!" << endl;
  if (USE_MULTIPLE_NODES)
    exec("bash killallgeths");
  else
    exec("killall geth");
  throw;

}

// /* Deploy contract using robot number i and return the transaction hash */
// std::string deploy_contract(int i, string contractPath) {
    
//   for (int trials = 0; trials < maxtrials; ++trials) {

//     if (DEBUG)
//       cout << "Trials is: " << trials << endl;

//     string txHashRaw = exec_geth_cmd(i, "loadScript(\"" + contractPath + "\")");
//     cout << "txHashRaw: " << txHashRaw << endl; 
//     string txHash;
//     std::istringstream f(txHashRaw);
//     std::getline(f, txHash);

//     cout << "txHash: " << txHash << endl; 

//     /* If a transaction hash was generated, i.e., neither true nor false were found */
//     if (txHash.find("true") == string::npos && txHash.find("false") == string::npos) {
// 	return txHash;
//       }
//     }  

//   /* If the maximum number of trials is reached */
//   cout << "Maximum number of trials is reached!" << endl;
//   exec("killall geth");
//   throw;
// }
 
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

/* Get the raw transaction based on the tx hash */
std::string getRawTransaction(int i, std::string txHash) {

  string cmd = "eth.getRawTransaction(" + txHash + ")";
  
  string rawTx = exec_geth_cmd(i, cmd);
  
  return rawTx;

}


/* Send raw transaction and include it in the tx pool */
std::string sendRawTransaction(int i, std::string rawTx) {

  string cmd = "eth.sendRawTransaction(" + rawTx + ")";

  string txHash = exec_geth_cmd(i, cmd);
  
  return txHash;

}


// Get number of white votes
int getWhiteVotes(int i){
  string cmd = "whiteVotes()";
  string res = exec_geth_cmd(i, cmd);
  
  int blockNumber = atoi(res.c_str());

  return blockNumber;

}

// Get number of black votes
int getBlackVotes(int i) {
  string cmd = "blackVotes()";
  string res = exec_geth_cmd(i, cmd);

  int resInt = atoi(res.c_str());

  return resInt;
}

// Get last 2 votes
int getLast2Votes(int i) {
  string cmd = "last2Votes()";
  string res = exec_geth_cmd(i, cmd);
  int resInt = atoi(res.c_str());

  return resInt;
}
