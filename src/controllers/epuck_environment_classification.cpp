/* Include the controller definit*io*itn */
#include "epuck_environment_classification.h"

#define ALPHA_CHANNEL		     0
#define COLOR_STRENGHT           255
#define N_COL		         	 3

#include <iostream>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <map>
//#include <thread>


/****************************************/
/****************************************/

using namespace std;

map<int, string> enodes;
map<int, string> coinbaseAddresses;
string interface; // Smart contract interface
int usedRack = 3;
int numNodes = 7;
SInt32 voteEveryXTicks = 5;
string username = "vstrobel";

/* Convert a number to a string */
template <typename T> std::string NumberToString ( T Number )
{
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

EPuck_Environment_Classification::EPuck_Environment_Classification() :
  m_pcWheels (NULL),
  m_fWheelVelocity (10.0f),
  m_pcRABA (NULL),
  m_pcRABS (NULL),
  m_cAlpha (10.0f),
  m_fDelta(0.5f),
  //m_pcLEDs(NULL),
  m_pcProximity(NULL),
  m_cGoStraightAngleRange(-ToRadians(m_cAlpha),
			  ToRadians(m_cAlpha)) {}

EPuck_Environment_Classification::CollectedData::CollectedData() :
  count (1) {}

EPuck_Environment_Classification::Opinion::Opinion() :
  countedCellOfActualOpinion (0)  {}

EPuck_Environment_Classification::Movement::Movement() :
  walkTime (3),
  actualDirection (0){}


/************************************************* INIT ********************************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::SimulationState::Init(TConfigurationNode& t_node) {

  try{
    /* Getting sigma, G value and the decision rule to follow */
    GetNodeAttribute(t_node, "g", g);
    GetNodeAttribute(t_node, "sigma", sigma);
    GetNodeAttribute(t_node, "lamda", LAMDA);
    GetNodeAttribute(t_node, "turn", turn);
    GetNodeAttribute(t_node, "decision_rule", decision_rule);
    GetNodeAttribute(t_node, "exitFlag", exitFlag);
    GetNodeAttribute(t_node, "percent_white", percentRed);		
    GetNodeAttribute(t_node, "percent_black", percentBlue);
    GetNodeAttribute(t_node, "num_pack_saved", numPackSaved);
    GetNodeAttribute(t_node, "base_dir", baseDir);
    GetNodeAttribute(t_node, "interface_path", interfacePath);
    GetNodeAttribute(t_node, "mapping_path", mappingPath);
    GetNodeAttribute(t_node, "use_multiple_nodes", useMultipleNodes);
    GetNodeAttribute(t_node, "use_background_geth_calls", useBackgroundGethCalls);
    GetNodeAttribute(t_node, "blockchain_path", blockchainPath);
    GetNodeAttribute(t_node, "base_port", basePort);
    GetNodeAttribute(t_node, "use_classical_approach", useClassicalApproach);
    GetNodeAttribute(t_node, "regenerate_file", regenerateFile);
    GetNodeAttribute(t_node, "profiling", profiling);
  }
  catch(CARGoSException& ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing controller state parameters.", ex);
  }
}


// Tell the smart contract the robot's public key
void EPuck_Environment_Classification::registerRobot() {

  int robotId = Id2Int(GetId());
  
  int args[1] = {(int) opinion.actualOpinion};
  int emptyArgs[0] = {};
  
  // Modify state of the blockchain
  smartContractInterfaceBg(robotId, interface,
	 contractAddress, "registerRobot", args, 1, 0, nodeInt, simulationParams.blockchainPath);
}



// Wait for the first event of the smart contract
void EPuck_Environment_Classification::updateRegistration() {

  int robotId = Id2Int(GetId());
  
  string eventResult;
  do {
   eventResult  = eventInterface(robotId, interface, contractAddress, nodeInt, simulationParams.blockchainPath);	
   } while (eventResult.find("Error") != string::npos);
  
  vector<string> splitResult = split(eventResult, ' ');    
  std::string sNewOpinion = splitResult[2];
  std::string sBlock = splitResult[1];
  std::string sBlockhash = splitResult[0];      
  cout << "sNewOpinion is " << sNewOpinion << endl;
  cout << "sBlock is " << sBlock << endl;
  cout << "sBlockhash is " << sBlockhash << endl;
  cout << "Registered robot" << endl;
  bwh.blockNumber = atoi(sBlock.c_str());
  bwh.hash = "\"" + sBlockhash + "\"";
  cout << "bwh.blockNumber: " << bwh.blockNumber << " bwh.hash:" << bwh.hash << endl;
}

void EPuck_Environment_Classification::Init(TConfigurationNode& t_node) {

  eventTrials = 0;
  receivedDecision = true;
  threadCurrentlyRunning = false;
  
  /* Initialize the actuators (and sensors) and the initial velocity as straight walking*/
  m_pcWheels = GetActuator<CCI_EPuckWheelsActuator>("epuck_wheels");
  m_pcProximity = GetSensor <CCI_EPuckProximitySensor>("epuck_proximity");
  //m_pcLEDs = GetActuator<CCI_LEDsActuator>("leds");
  //m_pcLEDs = GetActuator<CCI_EPuckBaseLEDsActuator>("epuck_base_leds");
  m_pcRABA = GetActuator<CCI_EPuckRangeAndBearingActuator>("epuck_range_and_bearing");
  m_pcRABS = GetSensor  <CCI_EPuckRangeAndBearingSensor>("epuck_range_and_bearing");
  m_pcRNG = CRandom::CreateRNG("argos");
  m_cGoStraightAngleRange.Set(-ToRadians(m_cAlpha), ToRadians(m_cAlpha));
  GetNodeAttributeOrDefault(t_node, "velocity", m_fWheelVelocity, m_fWheelVelocity);
  simulationParams.Init(GetNode(t_node, "simulation_parameters"));

  simulationParams.g = simulationParams.g * 10;
  simulationParams.sigma = simulationParams.sigma * 10;

  /* Colours read from robots could be changed and added here! AGGIUNGERECOLORI */
  red.Set(COLOR_STRENGHT,0,0,ALPHA_CHANNEL);      // Change alphachannel has not effect visively, but changing COLOR_STRENGHT could make
  green.Set(0,COLOR_STRENGHT,0,ALPHA_CHANNEL);    // cells more or less bright
  blue.Set(0,0,COLOR_STRENGHT,ALPHA_CHANNEL);

  /* Assign the initial state of the robots: all in exploration state*/
  m_sStateData.State = SStateData::STATE_EXPLORING;

  std::string m_strOutput;
  m_strOutput = GetId();

  /* IC it's an helping variable to read others opinion */
  IC.receivedOpinion = 5;
  IC.receivedQuality = 5;
  IC.senderID = 5;

  /* INITIAL QUALITY: has to be estimated in the first exploration state */
  opinion.quality = 0;

  /* Init REB actuators*/
  CCI_EPuckRangeAndBearingActuator::TData toSend;
  toSend[0]=5;
  toSend[1]=5;
  toSend[2]=5;
  toSend[3]=5;
  m_pcRABA->SetData(toSend);

  readNodeMapping();
  
  if(simulationParams.percentRed < simulationParams.percentBlue)
    simulationParams.percentRed = simulationParams.percentBlue;
  simulationParams.percentRed = simulationParams.percentRed / 100;

  cout << "Percent red is " << simulationParams.percentRed << endl;
  cout << "Percent blue is " << simulationParams.percentBlue << endl;
}

// Decide which robot runs on which cluster node
void EPuck_Environment_Classification::readNodeMapping() {

 int r_id;
 int r_node;

 ifstream infile;

 infile.open(simulationParams.mappingPath.c_str());
 
 while (infile >> r_id >> r_node)
   robotIdToNode[r_id] = r_node;

 infile.close();  
}


/* Connect/disconnect Ethereum processes to each other */
void EPuck_Environment_Classification::UpdateNeighbors(set<int> newNeighbors) {

  set<int> neighborsToAdd;
  set<int> neighborsToRemove;

  int robotId = Id2Int(GetId());
  
  /* Old neighbors minus new neighbors = neighbors that should be removed */
  std::set_difference(neighbors.begin(),
  		      neighbors.end(),
  		      newNeighbors.begin(),
  		      newNeighbors.end(),
  		      std::inserter(neighborsToRemove, neighborsToRemove.end()));


  /* New neighbors minus old neighbors = neighbors that should be added */
  std::set_difference(newNeighbors.begin(),
  		      newNeighbors.end(),
  		      neighbors.begin(),
  		      neighbors.end(),
  		      std::inserter(neighborsToAdd, neighborsToAdd.end()));
 
  
  std::set<int>::iterator it;
  for (it = neighbors.begin(); it != neighbors.end(); ++it) {
    int i = *it;
  }

  for (it = newNeighbors.begin(); it != newNeighbors.end(); ++it) {
    int i = *it;
  }

  for (it = neighborsToRemove.begin(); it != neighborsToRemove.end(); ++it) {
    
    int i = *it;
    if (simulationParams.useMultipleNodes) {
      string e = enodes[i];
      if (simulationParams.useBackgroundGethCalls)
	remove_peer_bg(robotId, e, nodeInt, simulationParams.blockchainPath);
      else
	remove_peer(robotId, e, nodeInt, simulationParams.blockchainPath);
    } else {
      remove_peer(robotId, get_enode(i));
    }
  }
   

  for (it = neighborsToAdd.begin(); it != neighborsToAdd.end(); ++it) {
    int i = *it;
    if (simulationParams.useMultipleNodes) {
      string e = enodes[i];
      if (simulationParams.useBackgroundGethCalls)
	add_peer_bg(robotId, e, nodeInt, simulationParams.blockchainPath);
      else
	add_peer(robotId, e, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);
    }
    else {
      add_peer(robotId, get_enode(i));
    }
  }
  
  // Update neighbor array
  set<int> neighborsTmp(newNeighbors);
  neighbors = neighborsTmp;
  
}


/************************************************* CONTROL STEP ************************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::ControlStep() {

  int robotId = Id2Int(GetId());

  if (!simulationParams.useClassicalApproach) {
    if (beginning) {
      start_mining_bg(robotId, 1, nodeInt, simulationParams.blockchainPath);
      updateRegistration();
      stop_mining_bg(robotId, nodeInt, simulationParams.blockchainPath);
      beginning = false;
    }
  }


  /* Start geth (just for debugging) */
  //system("geth");
  
  /* Turn leds according with actualOpinion */		
  TurnLeds();
	
  /* Move robots following randomWalk */
  Move();

  /* Two different behaviours, depending on if they are diffusing or exploring */
  switch(m_sStateData.State) {
    
  case SStateData::STATE_EXPLORING: {

    /* For a fully connected network (debugging) */
    set<int> currentNeighbors;
    
    // Fully connected
    //for (UInt8 i = 0; i <= 19; i++) {
    //  currentNeighbors.insert(i);
    //}

    if (!simulationParams.useClassicalApproach) {
      UpdateNeighbors(currentNeighbors);
      
      if (mining) {
	cout << " STOP MINING -- robot" << robotId << endl;
	mining = false;
	if (simulationParams.useMultipleNodes)
	  stop_mining_bg(robotId, nodeInt, simulationParams.blockchainPath);
	else
	  stop_mining(robotId);
      }
    }
    Explore();    
    break;
  }

  case SStateData::STATE_DIFFUSING: {  
    Diffusing();
    break;
  }
    
  }
  
  RandomWalk();

  /**** OBSTACLE AVOIDANCE ****/

  /* Get readings from proximity sensor and sum them together */
  const CCI_EPuckProximitySensor::TReadings& tProxReads = m_pcProximity->GetReadings();
  CVector2 cAccumulator;
  for(size_t i = 0; i < tProxReads.size(); ++i) {
    cAccumulator += CVector2(tProxReads[i].Value, tProxReads[i].Angle);
  }
  if(tProxReads.size()>0)
    cAccumulator /= tProxReads.size();
  /* If the angle of the vector is not small enough or the closest obstacle is not far enough curve a little */
  CRadians cAngle = cAccumulator.Angle();
  if(!(m_cGoStraightAngleRange.WithinMinBoundIncludedMaxBoundIncluded(cAngle) && cAccumulator.Length() < m_fDelta )) {
    /* Turn, depending on the sign of the angle */
    if(cAngle.GetValue() > 0.0f) {
      m_pcWheels->SetLinearVelocity( m_fWheelVelocity, 0.0f);
    }
    else {
      m_pcWheels->SetLinearVelocity(0.0f, m_fWheelVelocity);
    }
  }  
}

/************************************************** RANDOM WALK ************************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::RandomWalk() {


  /* walkTime represents the number of clock cycles (random number) of walk in a random direction*/
  if ( movement.walkTime == 0 )                            // Is the walkTime in that direction finished? ->
    { 				                       // -> YES: change direction//
      
      if ( movement.actualDirection == 0 )                  // If robot was going straight then turn standing in ->
	// -> a position for an uniformly distribuited time //
	{
	  CRange<Real> zeroOne(0.0,1.0);
	  Real p = m_pcRNG->Uniform(zeroOne);
	  p = p*simulationParams.turn;
	  Real dir = m_pcRNG->Uniform(CRange<Real>(-1.0,1.0));
	  if ( dir > 0 )
	    movement.actualDirection = 1;
	  else
	    movement.actualDirection = 2;
	  movement.walkTime = Floor(p);
	}
      
      else 						// The robot was turning, time to go straight for ->
	// -> an exponential period of time //
	{
	  movement.walkTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.LAMDA)*4); // Exponential random generator. *50 is a scale factor for the time
	  movement.actualDirection = 0;
	}
    }
  else {							// NO: The period of time is not finished, decrement the ->
    // -> walkTime and keep the direction //
    movement.walkTime--;
  }
}


/************************************************* EXPLORING STATE *********************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::Explore() {

  m_pcRABS->ClearPackets();
  
  int robotId = Id2Int(GetId());
  //  int nodeInt = robotIdToNode[robotId];
  
  /* remainingExplorationTime it's the variable decremented each control step. 
   * This variable represents the time that a robot must still spend in exploration state.
   * If this variable it's greater than zero, then it must be decremented and the robot should 
   * do exploration's stuffs (Update counters figuring out in which cell he is. It's done in loop function */
  if(m_sStateData.remainingExplorationTime > 0){		
    m_sStateData.remainingExplorationTime--;
  }

  /* If its time to change state, then the robot has to reset his own variables:
   * - Assign a new random exponential time: remainingExplorationTime and explorDurationTime (used to
   *   keep trace of the exploration times, just for statistic aims);
   * - Calculate the quality of the opinion, basing on the sensed datas (Number of counted cells of actual
   *   opinion / Number of total counted cells);
   * - Reset counting variables (countedCellOfActualOpinion and count [total number of cells counted]);
   * - Change state: Exploration->Diffusing;
   * - Generate a new Diffusing time (same as exploring, but used for Diffusing state and calculated with
   *   different params for the random variable;
   */
  else{
    if (byzantineStyle == 4 || byzantineStyle == 5)
      opinion.quality = 1.0;
    else
      opinion.quality = (Real)((Real)(opinion.countedCellOfActualOpinion)/(Real)(collectedData.count));
    
    
    opinion.countedCellOfActualOpinion = 0;
    receivedOpinions.clear();
    collectedData.count = 1;
    m_sStateData.State = SStateData::STATE_DIFFUSING;
    
    if (!simulationParams.useClassicalApproach) {
      uint opinionInt = (uint) (opinion.quality * 100); // Convert opinion quality to a value between 0 and 100
      string args[4] = {NumberToString(opinion.actualOpinion),
			NumberToString(simulationParams.decision_rule),
			NumberToString(bwh.blockNumber),
			bwh.hash}; 

      string voteResult;

      if (simulationParams.useMultipleNodes){

	int args3[1] = {bwh.blockNumber};
	smartContractInterfaceStringBg(robotId, interface, contractAddress, "vote", args, 4, opinionInt, nodeInt, simulationParams.blockchainPath);
      } 
    }
    
    /* Assigning a new exploration time, for the next exploration state */
    
    m_sStateData.remainingExplorationTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.sigma));
    m_sStateData.explorDurationTime = m_sStateData.remainingExplorationTime;

    /*
     * Assigning a new diffusing time for the incoming diffusing time, if the decision rule is the not-weighted
     * direct comparison then the next diffusing time is weighted with the ideal quality of the best opinion
     */

    if (simulationParams.decision_rule == 0 || simulationParams.decision_rule == 2) {

      m_sStateData.remainingDiffusingTime = (m_pcRNG->Exponential(((Real)simulationParams.g)*((Real)simulationParams.percentRed)))+30;

    } else if (simulationParams.decision_rule == 1 || simulationParams.decision_rule == 3) {

      m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.g*(Real)opinion.quality)+30);

    } else {

      /* Non-implemented decision rule */
      cout << "Unknown decision rule" << endl;

      if (!simulationParams.useClassicalApproach) {
	if (simulationParams.useMultipleNodes) {
	  //killGethAndRemoveFolders(simulationParams.blockchainPath, simulationParams.regenerateFile)
;	  cout << "epuck_environment_classification error !!" << endl;
	  throw;
	}
      }
      
      throw;

    }

    m_sStateData.diffusingDurationTime = m_sStateData.remainingDiffusingTime;
  }  
}

/************************************************* DIFFUSING STATE *********************************************/
/***************************************************************************************************************/

// Wait until a transaction is mined and the corresponding event is created
void EPuck_Environment_Classification::WaitForDecision() {

  int robotId = Id2Int(GetId());
  string eventResult;

  cout << "Robot id is " << robotId << endl;
  eventResult = eventInterface(robotId, interface, contractAddress, nodeInt, simulationParams.blockchainPath);	

  if (eventResult.find("Error") == string::npos) {

	vector<string> splitResult = split(eventResult, ' ');    
	std::string sNewOpinion = splitResult[2];
	std::string sBlock = splitResult[1];
	std::string sBlockhash = splitResult[0];      
	cout << "sNewOpinion is " << sNewOpinion << endl;
	cout << "sBlock is " << sBlock << endl;
	cout << "sBlockhash is " << sBlockhash << endl;
	

	if (bwh.blockNumber != atoi(sBlock.c_str())) {
      
	  int newOpinion = atoi(sNewOpinion.c_str());
	  bwh.blockNumber = atoi(sBlock.c_str());      
	  bwh.hash = "\"" + sBlockhash + "\"";
	  opinion.actualOpinion = newOpinion;


	  if (byzantineStyle > 0) {
	
	    switch(byzantineStyle) {
	    case 1 : opinion.actualOpinion = 1;
	      break;
	    case 2 : opinion.actualOpinion = 2;
	      break;
	    case 5 : opinion.actualOpinion = 2;
	      cout << "I am Byzantine id:" << robotId << "keeping opinion 2" << endl;
	      break;
	    default:
	      cout << "Wrong byzantine style" << endl;
	      throw;
	    }    
	}
      receivedDecision = true;
	}
      }
  threadCurrentlyRunning = false;
}

void EPuck_Environment_Classification::ConnectAndListen() {

	/* Every received data is stored in IC variable (helping
	 * var). Each IC variable will be inserted in receivedOpinions
	 * array if has not been sensed yet and it's not a 0,0,0 one.
	 * It will be used to choose the next opinion, basing on
	 * decision rules. After a decision has been taken this array
	 * will be emptied for the next diffusing state. */
	
	/* TODO: check!! Davide has this function in the next condition,
	   that is, (if( m_sStateData.remainingDiffusingTime < 30 )) */
  int robotId = Id2Int(GetId());
  set<int> currentNeighbors;
	
  const CCI_EPuckRangeAndBearingSensor::TPackets& tPackets = m_pcRABS->GetPackets();
	
  for(size_t i = 0; i < tPackets.size() ; ++i) {
	  
    bool saved = false;   // saved = variable to not save opinions twice: if saved == true -> don't save the datas
    
    /*
     * IC = Helping variable for sensed opinions, if the received opinion is 5 then not save it (5 is the default value
     * of the RAB actuators, if you receive 5 then the sender robot wasn't ready to send a new opinion
     */
    IC.receivedOpinion = tPackets[i]->Data[0];
    if(IC.receivedOpinion == 5)
      saved = true;
    
    IC.senderID = tPackets[i]->Data[3];
	  
    
    /* Loop for sense quality value: quality has been sent using 3 cells of RAB datas,
       so here it will converted in a Real number */
    IC.receivedQuality=0;
    for ( UInt32 j = 1; j<3 ; ++j)
      IC.receivedQuality = IC.receivedQuality*100 + tPackets[i]->Data[j];
    IC.receivedQuality = (Real) IC.receivedQuality / 10000;
    
    /* If the incoming value has already been listened then not save it */
    for(UInt32 j = 0; j < receivedOpinions.size(); ++j)
      if(receivedOpinions[j].senderID == IC.senderID)
	saved = true;
    
    /*
     * Don't want to save 0,0,0 values (values sent casually before to add 555 value, probably no
     * more used now.
     */
    
    /* Save value if it has not been already saved and it's not 5,5,5 or 0,0,0 value  */
    if(!saved) {
      if (simulationParams.useClassicalApproach)
	receivedOpinions.push_back(IC);
      else /* Update Ethereum neighbors */
	currentNeighbors.insert(IC.senderID);   	      
    }    
  }


  /* Listen to other opinions */
  /* In my implementation, robots in the diffusing state are
     connected with each other; since the blockchain is a
     bilateral protocol and one cannot only receive but not send
     or vice versa */
  
  if (!simulationParams.useClassicalApproach) {
    UpdateNeighbors(currentNeighbors);
	  

    //double before_mining = measure_time(begin_diffuse, "Update neighbors finished");
	
    if (!mining) {
      cout << " START MINING -- robot" << robotId << endl;
      mining = true;
      if (simulationParams.useMultipleNodes)
	start_mining_bg(robotId, 1, nodeInt, simulationParams.blockchainPath);
      else
	start_mining(robotId, 1);
    }    	  
  }
}


void EPuck_Environment_Classification::DiffuseInformation() {
  int robotId = Id2Int(GetId());

  /* LEDS must be lighted with intermittence in diffusing state */
  //if(m_sStateData.remainingDiffusingTime%3)
  // m_pcLEDs->SetAllColors(CColor::GREEN);

      /* Following things will have always to be done if in diffusing state. Here robot sends its opinion,
       * quality and ID. toSend it's the variable of TData type used from Epuck RAB to send datas. We will
       * prepare this variable before to send it. It has 4 Byte of datas
       */
      
      CCI_EPuckRangeAndBearingActuator::TData toSend;

      /* First Byte used for the opinion of the robot */
      toSend[0] = opinion.actualOpinion;

      /* Second and Third Byte used for the quality of the robots */
      Real p = opinion.quality;
      UInt8 t;
      for (UInt32 i = 0; i < 2; ++i)
	{
	  p = p*100;
	  t = (UInt8)p;
	  p = p - t;
	  toSend[i+1] = t;
	}

      /* Send ID: ID sent on the 4th Byte. An univoque transformation of the
       * string id is calculated by using the ASCII charachter of the string,
       * and then it will be sent to the other robots. (ep45 -> 45 is sent).
       */
      std::string id = GetId();
      UInt32 idConversion = id[2]-'0';
      if(id[3]!='\0')
	idConversion = (idConversion*10) + (id[3]-'0');

      toSend[3] = idConversion;

      /* Send datas and decrement the diffusing time left */
      m_pcRABA->SetData(toSend);

      /* Send opinion via Ethereum */
      if (!simulationParams.useClassicalApproach) {


	/* TODO: I think the direct modulation can be implemented
	   exactly as the other opinions, therefore, I'll keep that here
	   for a backup */
	if (simulationParams.decision_rule == 100) {
	  /* Send transaction to all neighbors */
	  /* TODO: I should check if this rule coul;d be really
	     implemented with real robots in a p2p way */
	  
	} else if (simulationParams.decision_rule == 2) {
	  
	  /* Don't do anything */
	  
	} else if ((simulationParams.decision_rule == 1 || simulationParams.decision_rule == 3)) {

	  if (m_sStateData.remainingDiffusingTime % voteEveryXTicks == 0) {

	    /* Create a transaction in each time step */    
	  
	    uint opinionInt = (uint) (opinion.quality * 100); // Convert opinion quality to a value between 0 and 100
	    //cout << "Opinion to send is " << (opinion.actualOpinion / 2) << endl;
	    string args[4] = {NumberToString(opinion.actualOpinion),
			      NumberToString(simulationParams.decision_rule),
			      NumberToString(bwh.blockNumber),
			      bwh.hash}; 
	  
	    if (simulationParams.useMultipleNodes)
	      smartContractInterfaceStringBg(robotId, interface, contractAddress, "vote", args, 4, opinionInt, nodeInt, simulationParams.blockchainPath);
	  }
	  
	} else if (simulationParams.decision_rule == 4) {
	  /* Switch blockchain versions, i.e., mine on a different
	     blockchain */

	  /* NOT YET IMPLEMENTED */
	  if (!simulationParams.useClassicalApproach) {
	    if (simulationParams.useMultipleNodes) {

	      cout << "epuck_environment_classification error !!" << endl;
	      throw;
	      //killGethAndRemoveFolders(simulationParams.blockchainPath, simulationParams.regenerateFile);	  
	      
	    }
	  }
	  
	  throw;
	  
	} else {

	  if (!simulationParams.useClassicalApproach) {
	    if (simulationParams.useMultipleNodes) {

	      cout << "epuck_environment_classification error !!" << endl;
	      throw;
	    }
	  }
	  
	  throw;
	}
      }

}

void EPuck_Environment_Classification::Diffusing() {

  m_sStateData.remainingDiffusingTime--;

  if (m_sStateData.remainingDiffusingTime < 30 ) {
    ConnectAndListen();
  }
  
  /* remainingDiffusingTime>0 means that is still time to perform diffusing state */
  if (m_sStateData.remainingDiffusingTime > 0) {

    DiffuseInformation();
    
    /* In the 3 lasts seconds (30 ticks) the robot starts listening to other opinions
     * and diffusing its own opinion, quality and ID */
    
  } else if (m_sStateData.remainingDiffusingTime == 0 || eventTrials > 20) {// Time to change to exploration state
	receivedDecision = false;
	DecisionRule(simulationParams.decision_rule);
	eventTrials = 0;

	if (simulationParams.useClassicalApproach) {
	  receivedDecision = true;
	}
	
	m_sStateData.remainingDiffusingTime--;
  } else if (m_sStateData.remainingDiffusingTime < 0 && !receivedDecision) {
    if (!threadCurrentlyRunning){
      eventTrials++;
      threadCurrentlyRunning = true;
      //thread t1(&EPuck_Environment_Classification::WaitForDecision, this);
      //#t1.detach();
    }
    //measure_time(begin_diffuse, "Decision rule application");
  } else if (m_sStateData.remainingDiffusingTime < 0 && receivedDecision) {

    eventTrials = 0;
    
    /* Reset exponential random diffusing time */
    UInt32 write = m_sStateData.diffusingDurationTime;
    m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.g*(Real)opinion.quality)+30);
    m_sStateData.diffusingDurationTime = m_sStateData.remainingDiffusingTime;
    
    /* Direct comparison without weighted diffusing time */
    if(simulationParams.decision_rule == 0 || simulationParams.decision_rule == 2)
      m_sStateData.remainingDiffusingTime = (m_pcRNG->Exponential(((Real)simulationParams.g)*((Real)simulationParams.percentRed)))+30;
    
    /* Change to EXPLORING state and choose another opinion with decision rules */
    m_sStateData.State = SStateData::STATE_EXPLORING;
    
    /* After decision has been taken, sensed values are deleted */
    receivedOpinions.clear();
    
    CCI_EPuckRangeAndBearingActuator::TData toSend;
    toSend[0]=5;
    toSend[1]=5;
    toSend[2]=5;
    toSend[3]=5;
    m_pcRABA->SetData(toSend);
    
  }
}

/* DECISION RULE */
void EPuck_Environment_Classification::DecisionRule(UInt32 decision_rule)
{
    
  if (simulationParams.useClassicalApproach) {

    if (byzantineStyle > 0) {
	
      switch(byzantineStyle) {
      case 1 : opinion.actualOpinion = 1;
	break;
      case 2 : opinion.actualOpinion = 2;
	break;
      case 5 : opinion.actualOpinion = 2;
	break;
      default:
	cout << "Wrong byzantine style" << endl;
	throw;
      }
    } else {
      
	switch(decision_rule) {

	case 0: {
	  NotWeightedDirectComparison();
	  break;
	}
	case 1: {
	  VoterModel();
	  break;
	}
	case 2: {
	  DirectComparison();
	  break;
	}
	case 3: {
	  MajorityRule();
	  break;
	}
	}
    }
    
  } else {

    int robotId = Id2Int(GetId());
    //start_mining_bg(robotId, 1, nodeInt, simulationParams.blockchainPath);
    uint opinionInt = (uint) (opinion.quality * 100);
    int args[3] = {(int) decision_rule, (int) opinion.actualOpinion, (int) opinionInt};
    int emptyArgs[0] = {};
    string sOpinionBlocknumberBlockhash;
    if (simulationParams.useMultipleNodes) {
      smartContractInterfaceBg(robotId, interface, contractAddress, "applyStrategy", args, 3, 0, nodeInt, simulationParams.blockchainPath);
    }
  }
}

void EPuck_Environment_Classification::NotWeightedDirectComparison(){

	size_t size = receivedOpinions.size();
	std::vector<informationCollected> opinionsValuated;  // Set of information collected in every diffusing states

	if(receivedOpinions.size()>simulationParams.numPackSaved){
	  for(size_t j=0; j<simulationParams.numPackSaved; j++){
	    opinionsValuated.push_back(receivedOpinions[size-1-j]);
	  }
	}
	else
	  for(size_t j=0; j<receivedOpinions.size(); j++)
	    opinionsValuated.push_back(receivedOpinions[j]);

	size = opinionsValuated.size();
	if(size > 0){
		CRange<Real> sizeRange(0,size);
		UInt32 index = (UInt32)Floor(m_pcRNG->Uniform(sizeRange));
		if ( opinionsValuated[index].receivedQuality > opinion.quality )
			opinion.actualOpinion = opinionsValuated[index].receivedOpinion;
	}
}
/* Randomly trust in one sensed opinion: receivedOpinions it's an array containing all the sensed opinions
 * relatively to the last diffusing state listening. With VoterModel, a robot randomly choose an opinion 
 * and uses it. 
 */

void EPuck_Environment_Classification::VoterModel(){

	size_t size = receivedOpinions.size();
	std::vector<informationCollected> opinionsValuated;  // Set of information collected in every diffusing states

	if(receivedOpinions.size()>simulationParams.numPackSaved){
		for(size_t j=0; j<simulationParams.numPackSaved; j++){
			opinionsValuated.push_back(receivedOpinions[size-1-j]);
		}
	}
	else
	        for(size_t j=0; j<receivedOpinions.size(); j++)
		    opinionsValuated.push_back(receivedOpinions[j]);

	size = opinionsValuated.size();

	if(size > 0){
		CRange<Real> sizeRange(0,size);
		UInt32 index = (UInt32)Floor(m_pcRNG->Uniform(sizeRange));
		opinion.actualOpinion = opinionsValuated[index].receivedOpinion;
	} 
}

/* Randomly trust in one sensed opinion: receivedOpinions it's an array containing all the sensed opinions
 * relatively to the last diffusing state listening. With DirecComparison, a robot randomly choose an opinion 
 * and compare this opinion with his previous one. If it's stronger he uses this opinion. 
 */
void EPuck_Environment_Classification::DirectComparison(){

	size_t size = receivedOpinions.size();
	std::vector<informationCollected> opinionsValuated;  // Set of information collected in every diffusing states

	if(receivedOpinions.size()>simulationParams.numPackSaved){
		for(size_t j=0; j<simulationParams.numPackSaved; j++){
			opinionsValuated.push_back(receivedOpinions[size-1-j]);
		}
	}
	else
	        for(size_t j=0; j<receivedOpinions.size(); j++)
		    opinionsValuated.push_back(receivedOpinions[j]);
	size = opinionsValuated.size();
	if(size > 0){
		CRange<Real> sizeRange(0,size);
		UInt32 index = (UInt32)Floor(m_pcRNG->Uniform(sizeRange));
		if ( opinionsValuated[index].receivedQuality > opinion.quality )
			opinion.actualOpinion = opinionsValuated[index].receivedOpinion;
	}
}

/* Randomly trust in one sensed opinion: receivedOpinions it's an array containing all the sensed opinions
 * relatively to the last diffusing state listening. With MajorityRule, a robot counts the opinions and uses
 * the more present one. 
 */
void EPuck_Environment_Classification::MajorityRule(){

	UInt32 numberOpinionsReceived[N_COL];
	std::vector<informationCollected> opinionsValuated;  // Set of information collected in every diffusing states
	IC.receivedOpinion=opinion.actualOpinion;

	size_t size = receivedOpinions.size();

	//cout << "receivedOpinions.size() is " << size << endl;
	
	if (receivedOpinions.size() > simulationParams.numPackSaved){
	  for(size_t j=0; j < simulationParams.numPackSaved; j++){
	    opinionsValuated.push_back(receivedOpinions[size-1-j]);
	  }
	} else {
	  for(size_t j=0; j<receivedOpinions.size(); j++) {
	    opinionsValuated.push_back(receivedOpinions[j]);
	  }

	  /* Add the robot's own opinion */
	  opinionsValuated.push_back(IC);
	  /* Setting majority array to 0 */

	  for ( UInt32 c = 0; c < N_COL; c++ ) {
	    numberOpinionsReceived[c] = 0;
	  }

	  /* For each received opinion, increment the correspondent cell. numberOpinionsReceived it's simply a contator for each color */
	  for ( size_t i = 0; i < opinionsValuated.size(); i++ ) {
	    numberOpinionsReceived[opinionsValuated[i].receivedOpinion]++;
	  }

	  opinion.actualOpinion = FindMaxOpinionReceived(numberOpinionsReceived, opinion.actualOpinion);
	  //opinion.actualOpinion = FindMaxOpinionReceivedWithBug(numberOpinionsReceived, opinion.actualOpinion);
	  }
}

UInt32 EPuck_Environment_Classification::FindMaxOpinionReceived(UInt32 numberOpinionsReceived[], UInt32 actualOpinion){

  /* TODO: There was a bug in here: in case of a tie, always white was
     favored, resulting in a exit probability of 0; for now, I've
     fixed the bug by returning the previous opinion in case of a
     tie. As soon as we use three different colors, we need a
     different approach: find out which colors contribute to the tie
     and either randomly break them if they are not part of the
     current opinion, or keep the current opinion if it's part of the
     tie */

  /* According to Davide, ties should be broken as follows: "If in the
     set of received opinions there is a tie, i.e., there is not a
     majority, then the robot keeps its own opinion." */
  
	UInt32 max = 0, index = 0;

	bool tie = false;
	
	for( UInt32 i = 0; i < N_COL; i++) {
	  
	  if( (i == 0) || numberOpinionsReceived[i] > max ) {
	    max = numberOpinionsReceived[i];
	    index = i;
	  } else if (numberOpinionsReceived[i] == max) {	    
	    tie = true;
	  }
	}
	if(max == 0 || tie)
	  return actualOpinion;
	else
	  return index;
}


UInt32 EPuck_Environment_Classification::FindMaxOpinionReceivedWithBug(UInt32 numberOpinionsReceived[], UInt32 actualOpinion){

	UInt32 max = 0, index = 0;

	for( UInt32 i = 0; i<N_COL; i++)
		if( numberOpinionsReceived[i] > max )
		{
			max = numberOpinionsReceived[i];
			index = i;
		}
	if(max == 0)
		return actualOpinion;
	else
		return index;
}



/************************************************* MOVEMENT ****************************************************/
/***************************************************************************************************************/
/* Implement the moviment leaded by the random walk (see loop_function) */
void EPuck_Environment_Classification::Move(){
  if(movement.actualDirection == 0) // Go straight
    m_pcWheels->SetLinearVelocity(m_fWheelVelocity,  m_fWheelVelocity);
  else
    if(movement.actualDirection == 1) // Turn right
      m_pcWheels->SetLinearVelocity(m_fWheelVelocity,  -m_fWheelVelocity);
    else
      if(movement.actualDirection == 2) // Turn left
	m_pcWheels->SetLinearVelocity(-m_fWheelVelocity,  m_fWheelVelocity);
}

/************************************************* TURNING LEDS ON *********************************************/
/***************************************************************************************************************
0 = BLACK/EX-RED;
1 = GREEN; 
2 = WHITE/EX-BLUE
AGGIUNGERECOLORI 
*/
void EPuck_Environment_Classification::TurnLeds(){

  // bool useRealRobot = false;
  // if (!useRealRobot){
  // switch(opinion.actualOpinion) {

  // case 1: {
  //   opinion.actualOpCol = CColor::WHITE;
  //   m_pcLEDs->SetAllColors(CColor::WHITE);
  //   break;
  // }
  // case 2: {
  //   opinion.actualOpCol = CColor::BLACK;
  //   m_pcLEDs->SetAllColors(CColor::BLACK);
  //   break;
  // }
  // case 3: {
  //   opinion.actualOpCol = CColor::GREEN;
  //   m_pcLEDs->SetAllColors(CColor::GREEN);
  //   break;
  // }
  // }
  // }
}

void EPuck_Environment_Classification::killGethAndRemoveFolders(string bcPath, string regenFile){

  // Kill all geth processes  
  string bckiller = "bash " + bcPath + "/bckillerccall";
  exec(bckiller.c_str());


  // Remove blockchain folders
  string rmBlockchainData = "rm -rf " + bcPath + "*";
  exec(rmBlockchainData.c_str());
  
  
  // Regenerate blockchain folders
  string regenerateFolders = "bash " + regenFile;
  exec(regenerateFolders.c_str());      
  
}


void EPuck_Environment_Classification::fromLoopFunctionResPrepare(){

  
  cout << "Called fromLoopFunctionRes" << endl;
  
  IC.receivedOpinion = 5;
  IC.receivedQuality = 5;
  IC.senderID = 5;

  opinion.countedCellOfActualOpinion = 0;
  opinion.quality = 0;
  collectedData.count = 1;

  CCI_EPuckRangeAndBearingActuator::TData toSend;
  toSend[0]=5;
  toSend[1]=5;
  toSend[2]=5;
  toSend[3]=5;
  m_pcRABA->SetData(toSend);
  m_pcRABS->ClearPackets();
  receivedOpinions.clear();

  TurnLeds();

  /* Assign the initial state of the robots: all in exploration state*/
  m_sStateData.State = SStateData::STATE_EXPLORING;

  /* Assign the exploration time (random generated) */
  m_sStateData.remainingExplorationTime = (m_pcRNG->Exponential((Real)simulationParams.sigma));
  m_sStateData.explorDurationTime = m_sStateData.remainingExplorationTime;

  cout << "ID Raw is: " << GetId() << endl;
  int robotId = Id2Int(GetId());

  beginning = true;
  
  /* Ethereum */
  if (!simulationParams.useClassicalApproach) {
    nodeInt = robotIdToNode[robotId];       
    interface = readStringFromFile(simulationParams.baseDir + simulationParams.interfacePath);
    
    /* Find out on which cluster node this robot's geth process should be executed */
    if (simulationParams.useMultipleNodes) {

      ostringstream genesisPathStream;
      genesisPathStream << "~/genesis/genesis" << simulationParams.basePort << ".json";
      string genesisPath = genesisPathStream.str();

      geth_init(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath, genesisPath);
      start_geth(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);
      createAccount(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);

      coinbaseAddresses[robotId] = getCoinbase(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);
      address = coinbaseAddresses[robotId];    
      
      prepare_for_new_genesis(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);      

      
    } else {
      geth_init(robotId);
      start_geth(robotId);
      createAccount(robotId);   
      enodes[robotId] = get_enode(robotId);
      coinbaseAddresses[robotId] = getCoinbase(robotId);
      address = coinbaseAddresses[robotId];    
      unlockAccount(robotId, "test");
    }
  }
}

void EPuck_Environment_Classification::fromLoopFunctionResStart(){

  if (!simulationParams.useClassicalApproach) {
    int robotId = Id2Int(GetId());
  
    ostringstream genesisPathStream;
    genesisPathStream << "~/genesis/genesis" << simulationParams.basePort << ".json";
    string genesisPath = genesisPathStream.str();
        
    geth_init(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath, genesisPath);
    start_geth(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);
    enodes[robotId] = get_enode(robotId, nodeInt, simulationParams.basePort, simulationParams.blockchainPath);
    enode = enodes[robotId];
    unlockAccount(robotId, "test", nodeInt, simulationParams.basePort, simulationParams.blockchainPath);
  }
}

/****************************************/
/****************************************/


REGISTER_CONTROLLER(EPuck_Environment_Classification, "epuck_environment_classification_controller")
