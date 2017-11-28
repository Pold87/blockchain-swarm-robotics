#include "restclient-cpp/restclient.h"

#include "ethereum_rpc_json.h"



void distributeEther(uint eth_amount) {

  json j, params;

  j["jsonrpc"] = "2.0";
  j["method"] = "eth_sendTransaction";
  j["id"] = 1;

  params["from"] = addresses.at(0); // Coinbase

  // Send some Ether to Each robot
  for (int i = 0; i < numRobots; i++) {
    params["to"] = addresses.at(i + 1);
    string v = constructBigNumber(eth_amount);

    params["value"] = v; 

    j["params"] = {params};

    handleRequest(j);

  }
}

void getAccounts() {
  json j, params;

  j["jsonrpc"] = "2.0";
  j["method"] = "eth_accounts";
  j["id"] = 1;
  //j["params"] = params;

  json ret = handleRequest(j);

  for (auto& address : ret["result"]) {
    addresses.push_back(address);
   }
}



void finalizeEpoch(uint robot) {
    json j, params;

  j["jsonrpc"] = "2.0";
  j["method"] = "eth_sendTransaction";
  j["id"] = 1;

  // 0 is coinbase (miner)
  params["from"] = addresses.at(robot + 1); 
  params["to"] = contractAddress;

  params["data"] = finalizeFunc;

  j["params"] = {params};
  
  // explicit conversion (serialization) of the JSON object to string
  std::string s = j.dump();

  // initialize RestClient
  RestClient::init();
  
  RestClient::Response r = RestClient::post("http://localhost:8545",
					    "text/json",
					    s);
  
  std::cout << s;
  std::cout << r.body;

  // deinit RestClient. After calling this you have to call RestClient::init()
  // again before you can use it
  RestClient::disable();
}


json handleRequest(json j) {

  // explicit conversion (serialization) of the JSON object to string
  std::string s = j.dump();

  // initialize RestClient
  RestClient::init();
  
  RestClient::Response r = RestClient::post("http://localhost:8545",
					    "text/json",
					    s);
  
  std::cout << s << std::endl;

  std::stringstream buffer;
  buffer << r.body;

  json ret;
  try {
  // Convert buffer string to json object
  ret = json::parse(buffer.str());
  std::cout << buffer.str() << std::endl;
  } catch (...) {
    std::cout << "Parse error: json library could not parse the return value of the RPC request" << std::endl;
    std::cout << buffer.str() << std::endl;
    ret = j;
  }
    
  // deinit RestClient. After calling this you have to call RestClient::init()
  // again before you can use it
  RestClient::disable();

  return ret;
  
}

string constructBigNumber(uint eth_amount) {

  string s = to_string(eth_amount);
  
  for (int i = 0; i < 18; i++) {
    s += "0";
  }

  return s;
} 


/* Create color opinion */
std::string constructData(int c) {

 std:string colorOpinion;

  if (c == 0)
    colorOpinion = "0000000000000000000000000000000000000000000000000000000000000000";
  else
    colorOpinion = "0000000000000000000000000000000000000000000000000000000000000001";
  
  // Color opinion
  return voteFuncHash + colorOpinion;
  
}


/* Send opinion via RPC call */
void sendOpinion(uint robot, uint opinion) {

  json j, params;

  j["jsonrpc"] = "2.0";
  j["method"] = "eth_sendTransaction";
  j["id"] = 1;

  // 0 is coinbase (miner)
  params["from"] = addresses.at(robot + 1); 
  params["to"] = contractAddress;

  params["data"] = constructata(opinion);

  //  j["params"] = {params, "latest"};
  j["params"] = {params};
  
  // explicit conversion (serialization) of the JSON object to string
  std::string s = j.dump();

  // initialize RestClient
  RestClient::init();
  
  RestClient::Response r = RestClient::post("http://localhost:8545",
					    "text/json",
					    s);
  
  std::cout << s;
  std::cout << r.body;

  // deinit RestClient. After calling this you have to call RestClient::init()
  // again before you can use it
  RestClient::disable();
  
  
}
