#ifndef ETHEREUM_RPC_JSON_H
#define ETHEREUM_RPC_JSON_H


// Easy JSON handling
#include "/home/volker/software/json/src/json.hpp"

using json = nlohmann::json;
using namespace std;


// All addresses of an account 
extern vector<std::string> addresses;

// Address of the mined contract
extern std::string contractAddress;

// first four bytes of the vote function
extern const std::string voteFuncHash;
extern const std::string finalizeFunc;

/* Send JSON request to Ethereum RPC  */
json handleRequest(json j);

/* Send eth_amount to each registered robot from the miner account */
void distributeEther(uint eth_amount);

/* Fill the addresses vector */
void getAccounts();

/* Reset the voting counts and start new epoch */
void finalizeEpoch(uint robot);

/* Convert ether to wei and return as string (decimal places are not
   supported!)*/
string constructBigNumber(uint eth_amount);

string constructData(int c);

void sendOpinion(uint robot, uint opinion);

#endif
