/* Include the controller definition */
#include "epuck_environment_classification.h"

#define ALPHA_CHANNEL		     0
#define COLOR_STRENGHT           255
#define N_COL		         	 3


#include "geth_static.h" /* Use geth from C++ */

#include <iostream>

#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <map>


/****************************************/
/****************************************/

using namespace std;

map<int, string> enodes;
map<int, string> coinbaseAddresses;
string interface; // Smart contract interface


EPuck_Environment_Classification::SNeighborData::SNeighborData() :
  neighbors(set<UInt8>()) {}


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
    GetNodeAttribute(t_node, "percent_red", percentRed);
    GetNodeAttribute(t_node, "percent_blue", percentBlue);
    GetNodeAttribute(t_node, "num_pack_saved", numPackSaved);
    GetNodeAttribute(t_node, "base_dir", baseDir);
    GetNodeAttribute(t_node, "interface_path", interfacePath);
  }
  catch(CARGoSException& ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing controller state parameters.", ex);
  }
}

void EPuck_Environment_Classification::Init(TConfigurationNode& t_node) {

  /* Initialize the actuators (and sensors) and the initial velocity as straight walking*/
  m_pcWheels = GetActuator<CCI_EPuckWheelsActuator>("epuck_wheels");
  m_pcProximity = GetSensor <CCI_EPuckProximitySensor>("epuck_proximity");
  m_pcLEDs = GetActuator<CCI_LEDsActuator>("leds");
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

  if(simulationParams.percentRed < simulationParams.percentBlue)
    simulationParams.percentRed = simulationParams.percentBlue;
  simulationParams.percentRed = simulationParams.percentRed / 100;


  /* Ethereum */

  int robotId = Id2Int(GetId());

  if (robotId == 0) {
    system("killall geth");     
    system("rm -rf ~/Documents/eth_data/*");     
    interface = readStringFromFile(simulationParams.baseDir + simulationParams.interfacePath);
  }
 
   
  geth_init(robotId);
  start_geth(robotId);
  createAccount(robotId);   
  enodes[robotId] = get_enode(robotId);
  coinbaseAddresses[robotId] = getCoinbase(robotId);
  address = coinbaseAddresses[robotId];

  unlockAccount(robotId, "test");
 
}


void EPuck_Environment_Classification::UpdateNeighbors(set<UInt8> newNeighbors) {

  set<UInt8> neighborsToAdd;
  set<UInt8> neighborsToRemove;

  int robotId = Id2Int(GetId());
  
  /* Old neighbors minus new neighbors = neighbors that should be removed */
  std::set_difference(m_sNeighborData.neighbors.begin(),
  		      m_sNeighborData.neighbors.end(),
  		      newNeighbors.begin(),
  		      newNeighbors.end(),
  		      std::inserter(neighborsToRemove, neighborsToRemove.end()));


  /* New neighbors minus old neighbors = neighbors that should be added */
  std::set_difference(newNeighbors.begin(),
  		      newNeighbors.end(),
  		      m_sNeighborData.neighbors.begin(),
  		      m_sNeighborData.neighbors.end(),
  		      std::inserter(neighborsToAdd, neighborsToAdd.end()));
 
  
  //cout << "Robot " << robotId << " Old neighbors: ";

  std::set<UInt8>::iterator it;
  for (it = m_sNeighborData.neighbors.begin(); it != m_sNeighborData.neighbors.end(); ++it) {
  //for (size_t i = 0; i < m_sNeighborData.neighbors.size(); ++i) {
  //for (auto i : m_sNeighborData.neighbors) {
    UInt8 i = *it;
    //cout << i << " ";
  }
  //cout << endl;

  //cout << "Robot " << robotId << " New neighbors: ";
  for (it = newNeighbors.begin(); it != newNeighbors.end(); ++it) {
  //  for (auto i : newNeighbors) {
    UInt8 i = *it;
    //cout << i << " ";
  }
  //cout << endl;
  
  //cout << "Robot " << robotId << " Removing neighbors: ";

  for (it = neighborsToRemove.begin(); it != neighborsToRemove.end(); ++it) {
    //for (auto i : neighborsToRemove) {
    UInt8 i = *it;
    //cout << i << " ";
    remove_peer(robotId, enodes[i]);
  }
  cout << endl;
    
  //cout << "Robot " << robotId << " Adding neighbors: ";

  for (it = neighborsToAdd.begin(); it != neighborsToAdd.end(); ++it) {
  // for (auto i : neighborsToAdd) {
    UInt8 i = *it;
    //cout << i << " ";
    add_peer(robotId, enodes[i]);
  }
  //cout << endl;
  
  //Update neighbor array
  m_sNeighborData.neighbors = newNeighbors;
  
}


/************************************************* CONTROL STEP ************************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::ControlStep() {

  int robotId = Id2Int(GetId());
  
  /* Turn leds according with actualOpinion */
  TurnLeds();

	
  /* Move robots following randomWalk */
  Move();

  /* Two different behaviours, depending on if they are diffusing or exploring */
  switch(m_sStateData.State) {

  case SStateData::STATE_EXPLORING: {

    /* If one wants to have a fully connected network */
    set<UInt8> currentNeighbors;
    
    //for (UInt8 i = 1; i <= 10; i++) {
    //  currentNeighbors.insert(i);
    //}
    UpdateNeighbors(currentNeighbors);


    if (mining) {
      cout << " STOP MINING -- robot" << robotId << endl;
      mining = false;
      stop_mining(robotId);     
    }

    
    Explore();
    break;
  }

  case SStateData::STATE_DIFFUSING: {

    if (!mining) {
      cout << " START MINING -- robot" << robotId << endl;
      mining = true;
      start_mining(robotId, 1);     

    }
      
    
    /* Every received data is stored in IC variable (helping var). Each IC variable will be
     * inserted in receivedOpinions array if has not been sensed yet and it's not a 0,0,0 one.
     * It will be used to choose the next opinion, basing on decision rules. After a decision
     * has been taken this array will be emptied for the next diffusing state. */
    const CCI_EPuckRangeAndBearingSensor::TPackets& tPackets = m_pcRABS->GetPackets();
    
    set<UInt8> currentNeighbors;
    
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
      
      /* Update Ethereum neighbors */
      /* TODO: this is wrong! If there is no neighbors it does not get updated accordingly */
      currentNeighbors.insert(IC.senderID);
      
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
      if((IC.senderID == 0) && (IC.receivedQuality==0) && (IC.receivedOpinion==0))
	saved = true;
      
      /* Save value if it has not been already saved and it's not 5,5,5 or 0,0,0 value  */
      if(!saved) {
	receivedOpinions.push_back(IC);
      }
      
    }
    
    UpdateNeighbors(currentNeighbors);
    
    
    Diffusing();
    break;
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
	  //std::cout<<" LMB "<< simulationParams.LAMDA <<" WT NEW "<< movement.walkTime;
	  //std::cout.flush();
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
    opinion.quality = (Real)((Real)(opinion.countedCellOfActualOpinion)/(Real)(collectedData.count));
    
    std::cout<<"Qual "<<opinion.quality<<std::endl;
    std::cout<<"Op "<<opinion.actualOpinion<<std::endl;
    std::cout<<"actOp "<<	opinion.countedCellOfActualOpinion <<std::endl;
    std::cout<<"Count "<< collectedData.count<<std::endl;
    
    opinion.countedCellOfActualOpinion = 0;
    receivedOpinions.clear();
    collectedData.count = 1;
    m_sStateData.State = SStateData::STATE_DIFFUSING;


    // Vote if direct modulation
    if (simulationParams.decision_rule == 2 || simulationParams.decision_rule == 3) {
      cout << "Direct Modulation OR Majority Voting" << std::endl;
      string contractAddressNoSpace = contractAddress;

      contractAddressNoSpace.erase(std::remove(contractAddressNoSpace.begin(), 
					       contractAddressNoSpace.end(), '\n'),
				   contractAddressNoSpace.end());


      uint opinionInt = (uint) (opinion.quality * 100);
      cout << "Opinion to send is " << (opinion.actualOpinion / 2) << endl;
      int args[2] = {opinion.actualOpinion / 2, opinionInt};
      
      // DEBUGGING: always vote for 1
      //int args[2] = {1, opinionInt};
      string voteResult = smartContractInterface(robotId, interface, contractAddressNoSpace, "vote", args, 2);

      int args2[0] = {};
      
      // For debugging
      string numWhite = smartContractInterface(robotId, interface, contractAddressNoSpace, "whiteVotes", args2, 0);
      string numBlack = smartContractInterface(robotId, interface, contractAddressNoSpace, "blackVotes", args2, 0);

      cout << "Num white votes is: " << numWhite << "Num Black votes is: " << numBlack << endl;
      
      }


    /* Assigning a new exploration time, for the next exploration state */
    m_sStateData.remainingExplorationTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.sigma));
    m_sStateData.explorDurationTime = m_sStateData.remainingExplorationTime;

    /*
     * Assigning a new diffusing time for the incoming diffusing time, if the decision rule is the not-weighted
     * direct comparison then the next diffusing time is weighted with the ideal quality of the best opinion
     */

    m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.g*(Real)opinion.quality)+30);

    cout << "Remaining diffusing time is: " << m_sStateData.remainingDiffusingTime << " and opinion is " << opinion.actualOpinion << endl;

    /* TODO: simulationParams.decision_rule==2 added by volker check if correct!!! */
    if(simulationParams.decision_rule==0 || simulationParams.decision_rule==2)
      m_sStateData.remainingDiffusingTime = (m_pcRNG->Exponential(((Real)simulationParams.g)*((Real)simulationParams.percentRed)))+30;

    m_sStateData.diffusingDurationTime = m_sStateData.remainingDiffusingTime;
  }
}

/************************************************* DIFFUSING STATE *********************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::Diffusing() {


  int robotId = Id2Int(GetId());
  
  /* remainingDiffusingTime>0 means that is still time to perform diffusing state */
  if (m_sStateData.remainingDiffusingTime > 0)
    {
      /* In the 3 lasts seconds (30 ticks) the robot starts listening other opinions
       * and diffusing his own opinion, quality and ID */
      if(  m_sStateData.remainingDiffusingTime < 30 )
	{
	  /* Listen to other opinions */
	  
	}

      /* LEDS must be lighted with intermittence in diffusing state */
      if(m_sStateData.remainingDiffusingTime%3)
	m_pcLEDs->SetAllColors(CColor::GREEN);

      /* Following things will have always to be done if in diffusing state. Here robot sends its opinion,
       * quality and ID. toSend it's the variable of TData type used from Epuck RAB to send datas. We will
       * prepare this variable before to send it. It has 4 Byte of datas
       */
      

      CCI_EPuckRangeAndBearingActuator::TData toSend;

      /* First Byte used for the opinion of the robot */
      toSend[0] = opinion.actualOpinion;

      cout << "Robot ID: " << robotId << "-- Actual opinion is: "  << toSend[0] <<  endl;
      
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


      // if (simulationParams.decision_rule == 3) {
      // string contractAddressNoSpace = contractAddress;

      // contractAddressNoSpace.erase(std::remove(contractAddressNoSpace.begin(), 
      // 					       contractAddressNoSpace.end(), '\n'),
      // 				   contractAddressNoSpace.end());


      // uint opinionInt = (uint) (opinion.quality * 100);
      // int args[2] = {opinion.actualOpinion / 2, opinionInt};
      // string voteResult = smartContractInterface(robotId, interface, contractAddressNoSpace, "vote", args, 2);
      // }
      
      m_sStateData.remainingDiffusingTime--;


    }
  else // Time to change to exploration state
    {
      
      //cout << robotId << " STOP MINING" << endl;
      //stop_mining(robotId);

      //cout << robotId << " REMOVE ALL NEIGHBORS" << endl;

      //set<UInt8> currentNeighbors;
      //UpdateNeighbors(currentNeighbors);
      
      /* Reset exponential random diffusing time */
      UInt32 write = m_sStateData.diffusingDurationTime;
      m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential((Real)simulationParams.g*(Real)opinion.quality)+30);
      m_sStateData.diffusingDurationTime = m_sStateData.remainingDiffusingTime;

      /* Direct comparison without weighted diffusing time */
      if(simulationParams.decision_rule==0)
	m_sStateData.remainingDiffusingTime = (m_pcRNG->Exponential(((Real)simulationParams.g)*((Real)simulationParams.percentRed)))+30;


      /* Change to EXPLORING state and choose another opinion with decision rules */
      m_sStateData.State = SStateData::STATE_EXPLORING;
      DecisionRule(simulationParams.decision_rule);

      //	 	for(size_t i=0; i<receivedOpinions.size();i++){
      //
      //			//			std::cout << receivedOpinions[i].senderID << "\t"
      //			//					<< receivedOpinions[i].receivedQuality << "\t"
      //			//					<< receivedOpinions[i].receivedOpinion << "\t"
      //			//					<< opinion.actualOpinion <<std::endl;
      //
      //		}

      //		epuckFile<<receivedOpinions.size()<<"\t"<<write<<std::endl;

      //		std::cout<<receivedOpinions.size()<<" ";
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

  int robotId = Id2Int(GetId());

  string contractAddressNoSpace = contractAddress;

  contractAddressNoSpace.erase(std::remove(contractAddressNoSpace.begin(), 
					   contractAddressNoSpace.end(), '\n'),
			       contractAddressNoSpace.end());


  uint opinionInt = (uint) (opinion.quality * 100);
  int args[4] = {decision_rule, opinion.actualOpinion / 2, opinionInt, simulationParams.numPackSaved};
  string sNewOpinion = smartContractInterface(robotId, interface, contractAddressNoSpace, "applyStrategy", args, 4);
  int newOpinion = atoi(sNewOpinion.c_str());
  opinion.actualOpinion = newOpinion * 2; // Is implemented as 0 and 1 in the smart contract
  
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

void EPuck_Environment_Classification::MajorityRule(){

  UInt32 numberOpinionsReceived[N_COL];
  std::vector<informationCollected> opinionsValuated;  // Set of information collected in every diffusing states
  IC.receivedOpinion = opinion.actualOpinion;

  size_t size = receivedOpinions.size();
  if(receivedOpinions.size()>simulationParams.numPackSaved){
    for(size_t j=0; j<simulationParams.numPackSaved; j++){
      opinionsValuated.push_back(receivedOpinions[size-1-j]);
    }
  }
  else
    for(size_t j=0; j<receivedOpinions.size(); j++)
      opinionsValuated.push_back(receivedOpinions[j]);
  opinionsValuated.push_back(IC);
  /* Setting majority array to 0 */
  for ( UInt32 c = 0; c < N_COL; c++ )
    numberOpinionsReceived[c] = 0;

  /* For each received opinion, increment the correspondent cell. numberOpinionsReceived it's simply a contator for each color */
  for ( size_t i = 0; i<opinionsValuated.size(); i++ )
    numberOpinionsReceived[opinionsValuated[i].receivedOpinion]++;

  //for( UInt32 i = 0; i<3; i++)
  // opinion.actualOpinion = FindMaxOpinionReceived(numberOpinionsReceived, opinion.actualOpinion);

}


UInt32 EPuck_Environment_Classification::FindMaxOpinionReceived(UInt32 numberOpinionsReceived[], UInt32 actualOpinion){

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

  switch(opinion.actualOpinion) {

  case 0: {

    opinion.actualOpCol = CColor::BLACK;
    m_pcLEDs->SetAllColors(CColor::BLACK);
    break;
  }
  case 1: {
    opinion.actualOpCol = CColor::GREEN;
    m_pcLEDs->SetAllColors(CColor::GREEN);
    break;
  }
  case 2: {
    opinion.actualOpCol = CColor::WHITE;
    m_pcLEDs->SetAllColors(CColor::WHITE);
    break;
  }
  }
}

void EPuck_Environment_Classification::fromLoopFunctionRes(){

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
}

/****************************************/
/****************************************/


REGISTER_CONTROLLER(EPuck_Environment_Classification, "epuck_environment_classification_controller")
