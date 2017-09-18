#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>

#include <time.h>
#include <sys/time.h>


#include "geth_static.h"

#define DEBUG true
#define USE_MULTIPLE_NODES true

using namespace std;

string datadir_base =  "~/Documents/eth_data/data";
const string genesis = "~/genesis/genesis1.json ";
const string genesisTemplate = "~/genesis/genesis_template.json ";
const int rpc_base_port = 8300;
const int ipc_base_port = 33500;
const int maxtrials = 3000;
const string rack = "3";
bool gethStaticErrorOccurred;


double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time,NULL)){
    //  Handle error
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

/*
  Convert a robot Id (fbxxx) to an integer (xxx)
*/
uint Id2Int(std::string id) {

  uint idConversion = id[2] - '0';
  if(id[3]!='\0')
    idConversion = (idConversion * 10) + (id[3] - '0');

    return idConversion;
}

std::string removeSpace(std::string str) {

  string noSpace = str;
	  
  noSpace.erase(std::remove(noSpace.begin(), 
			    noSpace.end(), '\n'),
		       noSpace.end());

  return noSpace;
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

/* Reads the first line from a file */
string readStringFromFile(string fileName){
  string s;
  ifstream infile;
  infile.open(fileName.c_str());
  getline(infile, s); // Saves the line in s.
  infile.close();

  return s;
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

  cout << "exec_geth_cmd called !!!! Exiting... Command was " << command << endl;
  throw;
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


string exec_geth_cmd(int i, string command, int nodeInt, string datadirBase){


  
  string res = exec_geth_cmd_helper(i, command, nodeInt, datadirBase);
  
  int trials = 20;
  // Retry to execute the command if it failed
  while ( (res.find("Fatal") != string::npos) || (res.find("Error") != string::npos)) {

    /* Reconstruct the full command stream */
    ostringstream fullCommandStream;
    
    /* Run geth command on this node  */
    fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";
    
    //ReplaceStringInPlace(command, "\"", "\\\"");
    
    fullCommandStream << "geth" <<  nodeInt << " --exec " << "'" << command << "'" << " attach " << datadirBase << i << "/" << "geth.ipc\"";
    
    std::string fullCommand = fullCommandStream.str();

    
    //cout << "exec_geth_cmd: " << fullCommand << endl;
    //cout << "Result of exec_geth_cmd: " << res << endl;    
    
    sleep(1); // Wait for a second and retry
    res = exec_geth_cmd_helper(i, command, nodeInt, datadirBase);
    
    if (trials == 0)
      break;
    
    trials --;
    
  }

  // TODO: if everything is implemented correctly and safely, I can
  //    let fatal errors occur both with Fatal error and normal
  //    errors if (res.find("Fatal") != string::npos || (res.find("Error") != string::npos)) {
  if (res.find("Fatal") != string::npos) {
    cout << "Fatal error!!!" << endl;
    cout << "res was " << res << endl;
    cout << "Called exec_geth_cmd with" << i << " " << command << endl; 
    
    sendMail(res);
    
    gethStaticErrorOccurred = true;
  }
  
  return res;

}


// Take a geth command, execute it on the selected robot, and return the result string
string exec_geth_cmd_helper(int i, string command, int nodeInt, string datadirBase){

  ostringstream fullCommandStream;

  /* Run geth command on this node  */
  fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";
  
  ReplaceStringInPlace(command, "\"", "\\\"");
  
  fullCommandStream << "geth" <<  nodeInt << " --exec " << "'" << command << "'" << " attach " << datadirBase << i << "/" << "geth.ipc\"";
  
  std::string fullCommand = fullCommandStream.str();

  //cout << "Command in helper is: " << fullCommand << endl;
  
  string res = exec(fullCommand.c_str());

  return res;  
}

string exec_geth_cmd_with_geth_restart(int i, string command, int nodeInt, int basePort, string datadirBase) {

  string res = exec_geth_cmd_helper(i, command, nodeInt, datadirBase);

  int trials = 20;
  // Retry to execute the command if it failed
  while ( (res.find("Fatal") != string::npos) || (res.find("Error") != string::npos)) {

    /* Reconstruct the full command stream */
    ostringstream fullCommandStream;
    
    /* Run geth command on this node  */
    fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";
    
    //ReplaceStringInPlace(command, "\"", "\\\"");
    
    fullCommandStream << "geth" <<  nodeInt << " --exec " << "'" << command << "'" << " attach " << datadirBase << i << "/" << "geth.ipc\"";
    
    std::string fullCommand = fullCommandStream.str();
    
    //cout << "exec_geth_cmd: " << fullCommand << endl;
    //cout << "Result of exec_geth_cmd: " << res << endl;    

    if (res.find("no such file") != string::npos) {
      /* Init geth again */
      start_geth(i, nodeInt, basePort, datadirBase);
    }
    
    sleep(1); // Wait for a second and retry
    res = exec_geth_cmd_helper(i, command, nodeInt, datadirBase);
    
    if (trials == 0)
      break;
    
    trials --;
    
  }

  // TODO: if everything is implemented correctly and safely, I can
  //    let fatal errors occur both with Fatal error and normal
  //    errors if (res.find("Fatal") != string::npos || (res.find("Error") != string::npos)) {
  if (res.find("Fatal") != string::npos) {
   
    cout << "Fatal error!!!" << endl;
    cout << "res was " << res << endl;
    cout << "Called exec_geth_cmd with" << i << " " << command << endl; 
    sendMail(res);
    gethStaticErrorOccurred = true;
  }
  
  return res;
  
}

void sendMail(string body){

  // string bodyToSend = body;
  
  // bodyToSend.erase(std::remove(bodyToSend.begin(), 
  // 			 bodyToSend.end(), '\n'),
  // 	     bodyToSend.end());


  ofstream out("mail.txt");
  out << body;
  out.close();
  
  ostringstream fullCommandStream;

  /* Run geth command on this node  */
  //fullCommandStream << "echo " << body << " | mail -s Clustererror volker.strobel87@gmail.com";
  cout << "Sending mail" << endl;
  fullCommandStream << "mail -s Clustererror volker.strobel87@gmail.com < mail.txt";
  std::string fullCommand = fullCommandStream.str();  
  system(fullCommand.c_str());

}


// Take a geth command, execute it on the selected robot, and return the result string
void exec_geth_cmd_background(int i, string command, int nodeInt, string datadirBase){

  ostringstream fullCommandStream;

  /* Run geth command on this node  */
  fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";
  
  ReplaceStringInPlace(command, "\"", "\\\"");

  /* The ampersand symol here makes the difference to the normal exec_geth_cmd function */
  fullCommandStream << "geth" <<  nodeInt << " --exec " << "'" << command << "'" << " attach " << datadirBase << i << "/" << "geth.ipc\"&";
  
  std::string fullCommand = fullCommandStream.str();


  /* TODO: I cannot check for errors here since it is a background
     call; maybe I can come up with some error handling */
  
  //  if (DEBUG)
  //cout << "exec_geth_cmd_background: " << fullCommand << endl;
  
  system(fullCommand.c_str());

}


void geth_init(int i) {
  cout << "Calling geth_init for robot " << i << endl;

  std::ostringstream datadirStream;

  datadirStream << "~/Documents/eth_data/data" << i;
  
  string str_datadir = datadirStream.str();
  
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

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


void geth_init(int i, int nodeInt, int basePort, string datadirBase, string genesisPath) {
  cout << "Calling geth_init for robot " << i << endl;

  std::ostringstream datadirStream;

  datadirStream << datadirBase << i;
  
  string str_datadir = datadirStream.str();
  ostringstream fullCommandStream;

  /* Run geth command on this node  */
  fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";  
  fullCommandStream << "geth" << nodeInt << " --verbosity 3" << " --datadir " << str_datadir << " init " << genesisPath << "\"";
  
  string commandStream = fullCommandStream.str();

  if (DEBUG)
    cout << "geth init: " << commandStream << endl; 
  
  exec(commandStream.c_str());
  sleep(1);
    
}

void start_geth(int i) {
  sleep(1);

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
  datadirStream << datadir_base << i << + "/";
  
  string str_datadir = datadirStream.str();
  ostringstream portStream;

  portStream << " --port " << (ipc_base_port + i);
  string str_port = portStream.str();

  // Make folders
  string mk_folders = "mkdir -p " + str_datadir;
  
  system(mk_folders.c_str());

  // Compose geth command

  string ipc_path = "--ipcpath " + str_datadir + "geth.ipc";

  fullCommandStream << ipc_path << " --datadir " << str_datadir << str_port << " --maxpeers 130";

  //if (USE_MULTIPLE_NODES)
  //  ReplaceStringInPlace(geth_command, "\"", "\\\"");
  
  //fullCommandStream << geth_command;  

  if (USE_MULTIPLE_NODES)
    fullCommandStream << "\"";

  fullCommandStream << "&";
  
  cout << "Running command " << fullCommandStream.str() << endl;
  
  FILE* pipe = popen(fullCommandStream.str().c_str(), "r");
  pclose(pipe);
  sleep(1);
	  
}

void start_geth(int i, int nodeInt, int basePort, string datadirBase) {
  
  sleep(1);
  cout << "Starting geth for robot " << i << endl;

  ostringstream fullCommandStream;
  
  /* Run geth command on this node  */
  fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";
  fullCommandStream << "geth" << nodeInt << " --verbosity 3 --networkid 2 --nodiscover ";
  
  std::ostringstream datadirStream;
  datadirStream << datadirBase << i << + "/";
  
  string str_datadir = datadirStream.str();
  ostringstream portStream;

  portStream << " --port " << (basePort + i);
  string str_port = portStream.str();

  // Make folders
  string mk_folders = "mkdir -p " + str_datadir;
  
  system(mk_folders.c_str());

  // Compose geth command

  string ipc_path = "--ipcpath " + str_datadir + "geth.ipc";

  fullCommandStream << ipc_path << " --datadir " << str_datadir << str_port << " --maxpeers 130" << "\"" << "&";

  cout << "Running command " << fullCommandStream.str() << endl;
  
  FILE* pipe = popen(fullCommandStream.str().c_str(), "r");
  pclose(pipe);
  sleep(1);
}


void createAccount(int i) {
    sleep(1);
  std::ostringstream fullCommandStream;
  fullCommandStream << "personal.newAccount(\"test\")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

}

void createAccount(int i, int nodeInt, int basePort, string datadirBase) {
  sleep(1);
  string cmd = "personal.newAccount(\"test\")";
  string res = exec_geth_cmd_with_geth_restart(i, cmd, nodeInt, basePort, datadirBase);
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

string get_enode(int i, int nodeInt, int basePort, string datadirBase) {

  string cmd = "admin.nodeInfo.enode";
  string res = exec_geth_cmd_with_geth_restart(i, cmd, nodeInt, basePort, datadirBase);

  // Print the received enode
  cout << "The enode is " << res << endl;

  ostringstream nodeStream;
  nodeStream << "c" << rack << "-" << nodeInt;
  string node = nodeStream.str();
  
  /* Resolve the hostname to its ip */
  string ip = hostname2ip(node);
  /* Replace [::] (localhost) with actual ip address */
  replace(res, "[::]", ip);
  
  // Print the replaced enode address
  cout << "The enode with the actual ip address is " << res << endl;
  
  return res;  
}


/* Start the mining process for robot i using t threads */
string start_mining(int i, int t) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "miner.start(" << t << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  return res;
}


/* Start the mining process for robot i using t threads */
string start_mining(int i, int t, int nodeInt, std::string datadirBase) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "miner.start(" << t << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);

  return res;

}

/* Start the mining process for robot i using t threads */
void start_mining_bg(int i, int t, int nodeInt, std::string datadirBase) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "miner.start(" << t << ")";
  string cmd = fullCommandStream.str();
  exec_geth_cmd_background(i, cmd, nodeInt, datadirBase);

}


/* Stop the mining process for robot i */
string stop_mining(int i) {
  string cmd = "miner.stop()";
  string res = exec_geth_cmd(i, cmd);
  return res;
}

/* Stop the mining process for robot i */
string stop_mining(int i, int nodeInt, std::string datadirBase) {
  string cmd = "miner.stop()";
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);
  return res;
}

/* Stop the mining process for robot i */
void stop_mining_bg(int i, int nodeInt, std::string datadirBase) {
  string cmd = "miner.stop()";
  exec_geth_cmd_background(i, cmd, nodeInt, datadirBase);
}


// Add enode to robot i
string add_peer(int i, string enode) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.addPeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  //if (DEBUG)
  //  cout << "add_peer: enode is " << enode << "result is" << res << endl;

  return res;
}

// Add enode to robot i
string add_peer(int i, string enode, int nodeInt, int basePort, string datadirBase) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.addPeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);

  //if (DEBUG)
  //  cout << "add_peer: enode is " << enode << "result is" << res << endl;

  return res;
}


// Add enode to robot i
void add_peer_bg(int i, string enode, int nodeInt, string datadirBase) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.addPeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  exec_geth_cmd_background(i, cmd, nodeInt, datadirBase);
}


// Remove enode from robot i
string remove_peer(int i, string enode) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.removePeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd);

  return res;
}

// Remove enode from robot i
string remove_peer(int i, string enode, int nodeInt, string datadirBase) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.removePeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);

  return res;
}



// Remove enode from robot i
void remove_peer_bg(int i, string enode, int nodeInt, string datadirBase) {

  std::ostringstream fullCommandStream;
  fullCommandStream << "admin.removePeer(" << enode << ")";
  string cmd = fullCommandStream.str();
  exec_geth_cmd_background(i, cmd, nodeInt, datadirBase);
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

// Get coinbase address of robot i
std::string getCoinbase(int i, int nodeInt, int basePort, string datadirBase){
  std::ostringstream fullCommandStream;
  fullCommandStream << "eth.coinbase";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd_with_geth_restart(i, cmd, nodeInt, basePort, datadirBase);

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

// Send v ether from robot i to address to
std::string sendEther(int i, std::string from, std::string to, int v, int nodeInt, string datadirBase) {
  std::ostringstream fullCommandStream;
  fullCommandStream << "eth.sendTransaction({from:"  << from << ",to:" << to << ", value: web3.toWei(" << v << ", \"ether\")})";
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);

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
    gethStaticErrorOccurred = true;
    //if (USE_MULTIPLE_NODES) {
    //  exec("bash killallgeths");
    //} else { 
    //  exec("killall geth");
    //}
    //throw;
  }

  return res;
}


// Get contract address from transaction receipt
std::string getContractAddress(int i, std::string txHash, int nodeInt, string datadirBase) {
  std::ostringstream fullCommandStream;

  cout << "txHash is: " << txHash << endl;
  
  fullCommandStream << "eth.getTransactionReceipt(\"" << txHash << "\").contractAddress";
  
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);

  //  if (DEBUG)
  //  cout << "DEBUG -- getContractAddress: " << res << endl;
    
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


/* Unlock account */
std::string unlockAccount(int i, std::string pw, int nodeInt, int basePort, string datadirBase) {
  std::ostringstream fullCommandStream;

  fullCommandStream << "personal.unlockAccount(eth.coinbase, \"" << pw << "\", 0)";
  
  string cmd = fullCommandStream.str();
  string res = exec_geth_cmd_with_geth_restart(i, cmd, nodeInt, basePort, datadirBase);

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
    fullCommandStream << "ps ax | grep \\\"\\-\\-port " << port << "\\\"";
    fullCommandStream << "\"";
    
  } else {
    fullCommandStream << "ps ax | grep \"\\-\\-port " << port << "\"";
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


void kill_geth_thread(int i, int basePort, int nodeInt, string datadirBase) { 

  int port = basePort + i;

  std::ostringstream fullCommandStream;

  /* Run geth command on this node  */
  fullCommandStream << "ssh vstrobel@c" << rack << "-" << nodeInt << " \"";
  fullCommandStream << "ps ax | grep \\\"\\-\\-port " << port << "\\\"" << "\"";
  
  string cmd = fullCommandStream.str();
  string res = exec(cmd.c_str());
  //system(cmd.c_str());
  
  //if (DEBUG)
  //  cout << "DEBUG -- kill_geth_thread (1st call) -- command:" << cmd << " result: " << res << endl;

  /* Only get the first word, i.e., the PID from the command */
  istringstream iss(res);
  std::string pid;

  iss >> pid;

  string cmd2;
  std::ostringstream fullCommandStream2;
  fullCommandStream2 << "ssh vstrobel@c" << rack << "-" << nodeInt << " ";
  fullCommandStream2 << "kill -HUP " << pid;
  cmd2 = fullCommandStream2.str();

  //res = exec(cmd2.c_str());
  system(cmd2.c_str());

  
  //if (DEBUG)
  //  cout << "DEBUG -- kill_geth_thread (2nd call) -- command:" << cmd2 << " result: " << res << endl;


  /* Kill again locally */
  std::string pid2;
  string cmd3;
  std::ostringstream fullCommandStream3;
  iss.ignore(numeric_limits<streamsize>::max(), '\n');
  iss >> pid2;
  fullCommandStream3 << "kill -HUP " << pid2;
  cmd3 = fullCommandStream3.str();

  //res = exec(cmd3.c_str());
  system(cmd3.c_str());

  //if (DEBUG)
  //  cout << "DEBUG -- kill_geth_thread (3rd call) -- command:" << cmd3 << " result: " << res << endl;  
  
  /* And again via SSH */
  string cmd4;
  std::ostringstream fullCommandStream4;
  fullCommandStream4 << "ssh vstrobel@c" << rack << "-" << nodeInt << " ";
  fullCommandStream4 << "kill -HUP " << pid2;
  cmd4 = fullCommandStream4.str();
  //res = exec(cmd4.c_str());
  system(cmd4.c_str());
  
  //if (DEBUG)
  //  cout << "DEBUG -- kill_geth_thread (4th call) -- command:" << cmd4 << " result: " << res << endl;
  
  //return res;
  
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

// Interact with a function of a smart contract
// v: Amount of wei to send
std::string smartContractInterface(int i, string interface, string contractAddress,
				   string func, int args[], int argc, int v, int nodeInt, string datadirBase) {
  
  
  ostringstream fullCommandStream;

  fullCommandStream << "var cC = web3.eth.contract(" << interface << ");var c = cC.at(" << contractAddress << ");c." << func << "(";

  
  for(int k = 0; k < argc; k++) {
    fullCommandStream << args[k] << ",";  
  }
  
  fullCommandStream << "{" << "value: " << v << ", from: eth.coinbase, gas: '1000000'});";
  
  
   std::string fullCommand = fullCommandStream.str();

   string res = exec_geth_cmd(i, fullCommand, nodeInt, datadirBase);
   cout << "Result received from SC is: " << res << endl;

   return res;
 }


// Interact with a function of a smart contract
// v: Amount of wei to send
std::string smartContractInterfaceCall(int i, string interface, string contractAddress,
				   string func, int args[], int argc, int v, int nodeInt, string datadirBase) {
  
  
  ostringstream fullCommandStream;

  fullCommandStream << "var cC = web3.eth.contract(" << interface << ");var c = cC.at(" << contractAddress << ");c." << func << ".call(";

  
  for(int k = 0; k < argc; k++) {
    fullCommandStream << args[k] << ",";  
  }
  
  fullCommandStream << "{" << "value: " << v << ", from: eth.coinbase, gas: '1000000'});";
  
  
   std::string fullCommand = fullCommandStream.str();

   string res = exec_geth_cmd(i, fullCommand, nodeInt, datadirBase);
   cout << "Result received from SC is: " << res << endl;

   return res;
 }


// Interact with a function of a smart contract
// v: Amount of wei to send
void smartContractInterfaceBg(int i, string interface, string contractAddress,
				   string func, int args[], int argc, int v, int nodeInt, string datadirBase) {
  
  
  ostringstream fullCommandStream;

  fullCommandStream << "var cC = web3.eth.contract(" << interface << ");var c = cC.at(" << contractAddress << ");c." << func << "(";

  
  for(int k = 0; k < argc; k++) {
    fullCommandStream << args[k] << ",";  
  }
  
  fullCommandStream << "{" << "value: " << v << ", from: eth.coinbase, gas: '1000000'});";
  
  
   std::string fullCommand = fullCommandStream.str();

   exec_geth_cmd_background(i, fullCommand, nodeInt, datadirBase);
   //cout << "Result received from SC is: " << res << endl;

 }


// Interact with a function of a smart contract
// v: Amount of wei to send
void smartContractInterfaceStringBg(int i, string interface, string contractAddress,
				   string func, string args[], int argc, int v, int nodeInt, string datadirBase) {
  
  
  ostringstream fullCommandStream;

  fullCommandStream << "var cC = web3.eth.contract(" << interface << ");var c = cC.at(" << contractAddress << ");c." << func << "(";

  
  for(int k = 0; k < argc; k++) {
    fullCommandStream << args[k] << ",";  
  }
  
  fullCommandStream << "{" << "value: " << v << ", from: eth.coinbase, gas: '1000000'});";
  
  
   std::string fullCommand = fullCommandStream.str();

   exec_geth_cmd_background(i, fullCommand, nodeInt, datadirBase);
   //cout << "Result received from SC is: " << res << endl;

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
  //if (USE_MULTIPLE_NODES)
  //  exec("bash killallgeths");
  //else
  //  exec("killall geth");
  //throw;
  gethStaticErrorOccurred = true;

}

/* Deploy contract using robot number i and return the transaction hash */
std::string deploy_contract(int i, string interfacePath, string dataPath, string templatePath, int nodeInt, string datadirBase) {

  // Get smart contract interface
  string interface = readStringFromFile(interfacePath);
  string data = readStringFromFile(dataPath);
  data = "0x" + data; // Make data hexadecimal
  string contractTemplate = readStringFromFile(templatePath);

  replace(contractTemplate, "INTERFACE", interface);
  replace(contractTemplate, "DATA", data);

  string tmpPath = datadirBase + "/tmp.txt";

  std::ofstream out(tmpPath.c_str());
  out << contractTemplate;
  out.close();

  cout << contractTemplate << std::endl;

  for (int trials = 0; trials < maxtrials; ++trials) {

    if (DEBUG)
      cout << "Trials is: " << trials << endl;

    string txHashRaw = exec_geth_cmd(i, "loadScript(\"" + tmpPath + "\")", nodeInt, datadirBase);
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

  //string bckiller = "bash " + datadirBase + "/bckillerccall";
  //exec(bckiller.c_str());    
  
  //throw;
  gethStaticErrorOccurred = true;
}


/* Check account balance of robot i (in wei)*/
int check_balance(int i) {
  string cmd = "eth.getBalance(eth.coinbase)";
  string res = exec_geth_cmd(i, cmd);
  long balance;
  istringstream ss(res);
  ss >> balance;

  return balance;
}


/* Check account balance of robot i (in wei) */
long long check_balance(int i, int nodeInt, string datadirBase) {
  string cmd = "eth.getBalance(eth.coinbase)";
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);  
  long long balance;// = atoi(res.c_str());

  if (res.find("true") != string::npos || res.find("Error") != string::npos) {
    balance = 0;
  } else {  
    istringstream ss(res);
    ss >> balance;
  }
  
  return balance;
}


// Get blockchain length of robot i
int getBlockChainLength(int i) {
  
  string cmd = "eth.blockNumber";
  string res = exec_geth_cmd(i, cmd);  
  int blockNumber;// = atoi(res.c_str());
  istringstream ss(res);
  ss >> blockNumber;
  

  return blockNumber;

}

// Get blockchain length of robot i
int getBlockChainLength(int i, int nodeInt, string datadirBase) {
  
  string cmd = "eth.blockNumber";
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);  
  int blockNumber;

  /* TODO: A check would be better. Sometime eth.blockNumber returns
     true which is converted to 32515 */
  if (res.find("true") != string::npos) {
    cout << "Result in getBlockChainLength was " << res << endl;
    blockNumber = -1;
  } else {  
    istringstream ss(res);
    ss >> blockNumber;
  }

  
  return blockNumber;

}


/* Get the raw transaction based on the tx hash */
std::string getRawTransaction(int i, std::string txHash) {

  string cmd = "eth.getRawTransaction(" + txHash + ")";  
  string rawTx = exec_geth_cmd(i, cmd);
  
  return rawTx;
}


/* Get the raw transaction based on the tx hash */
std::string getRawTransaction(int i, std::string txHash, int nodeInt, std::string datadirBase) {

  string cmd = "eth.getRawTransaction(" + txHash + ")";  
  string rawTx = exec_geth_cmd(i, cmd, nodeInt, datadirBase);
  
  return rawTx;
}



/* Send raw transaction and include it in the tx pool */
std::string sendRawTransaction(int i, std::string rawTx) {

  string cmd = "eth.sendRawTransaction(" + rawTx + ")";
  string txHash = exec_geth_cmd(i, cmd);
  
  return txHash;
}


/* Send raw transaction and include it in the tx pool */
std::string sendRawTransaction(int i, std::string rawTx, int nodeInt, std::string datadirBase) {

  string cmd = "eth.sendRawTransaction(" + rawTx + ")";
  string txHash = exec_geth_cmd(i, cmd, nodeInt, datadirBase);
  
  return txHash;
}



// Get number of white votes
int getWhiteVotes(int i){
  string cmd = "whiteVotes()";
  string res = exec_geth_cmd(i, cmd);
  
  int blockNumber;// = atoi(res.c_str());
  istringstream ss(res);
  ss >> blockNumber;


  return blockNumber;

}


// Get number of white votes
int getWhiteVotes(int i, int nodeInt, string datadirBase){
  string cmd = "whiteVotes()";
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);  
  int blockNumber;// = atoi(res.c_str());
  istringstream ss(res);
  ss >> blockNumber;


  return blockNumber;

}


// Get number of black votes
int getBlackVotes(int i) {
  string cmd = "blackVotes()";
  string res = exec_geth_cmd(i, cmd);

  int resInt;// = atoi(res.c_str());
  istringstream ss(res);
  ss >> resInt;


  return resInt;
}

// Get number of black votes
int getBlackVotes(int i, int nodeInt, string datadirBase) {
  string cmd = "blackVotes()";
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);
  istringstream ss(res);  
  int resInt;// = atoi(res.c_str());
  ss >> resInt;  

  return resInt;
}

// Get last 2 votes
int getLast2Votes(int i) {
  string cmd = "last2Votes()";
  string res = exec_geth_cmd(i, cmd);
  int resInt;// = atoi(res.c_str());
  istringstream ss(res);
  ss >> resInt;

  return resInt;
}


// Get last 2 votes
int getLast2Votes(int i, int nodeInt, string datadirBase) {
  string cmd = "last2Votes()";
  string res = exec_geth_cmd(i, cmd, nodeInt, datadirBase);
  int resInt;// = atoi(res.c_str());
  istringstream ss(res);
  ss >> resInt;


  return resInt;
}

/* Measure time and print it */
double measure_time(double ref_time, string part_name) {

  string alarm = "";
  double diff = get_wall_time() - ref_time;

  if (diff > 0.5)
    alarm = " ALARM";
  
  cout << fixed << part_name << " took (ms): " << diff << alarm << endl;

  return get_wall_time();
  
}

void generate_genesis(std::string address, int basePort) {

  ostringstream fullCommandStream;

  string addressNoSpace = address;
	  
  addressNoSpace.erase(std::remove(addressNoSpace.begin(), 
				   addressNoSpace.end(), '\n'),
		       addressNoSpace.end());
  
  cout << "Generating new genesis file" << endl;
  fullCommandStream << "bash replace_genesis.sh " << addressNoSpace << " " << basePort;
  std::string fullCommand = fullCommandStream.str();
  cout << fullCommand << endl;
  system(fullCommand.c_str());
}


void prepare_for_new_genesis(int i, int nodeInt, int basePort, string blockchainPath) {

  /* Kill miner geth thread to replace genesis block */
  kill_geth_thread(i, basePort, nodeInt, blockchainPath);
  /* And a second time (since ssh creates two processes) */
  kill_geth_thread(i, basePort, nodeInt, blockchainPath);
  
  /* Remove all files except for the keystore */
  std::ostringstream rmFilesStream;
  rmFilesStream << "rm -rf " << blockchainPath << i << "/geth*";
  std::string rmFilesStreamCmd = rmFilesStream.str();
  system(rmFilesStreamCmd.c_str());
}
