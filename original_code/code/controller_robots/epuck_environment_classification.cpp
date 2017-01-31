/**
 * @file </EC_Cluster/controllers/epuck_environment_classification.cpp>
 *
 * In this class we'll implement all the methods and, moreover, the control step of the robots.
 * With this class we define the controller specific for our experiment: an e-puck able to operate
 * in two states, to communicate with the rest of the swarm and to let the user visualize his behaviour
 * through leds.
 * This controller can be compiled to run with ARGoS simulator or cross compiled to run on the real robot.
 *
 * @author Davide Brambilla - <davide14.brambilla@polimi.it>
 */

#include "epuck_environment_classification.h"

#define ALPHA_CHANNEL 0
#define COLOR_STRENGHT 255
#define N_COLORS 3
#define SCALE 10
#define DEFAULT_VALUE_REB 5
#define LISTENING_TIME 30

EPuck_Environment_Classification::EPuck_Environment_Classification() :
    m_pcWheels (NULL),
    m_fWheelVelocity (10.0f),
    m_pcRABA (NULL),
    m_pcRABS (NULL),
    m_cAlpha (10.0f),
    m_fDelta(0.5f),
    m_pcProximity(NULL),

m_cGoStraightAngleRange(-ToRadians(m_cAlpha), ToRadians(m_cAlpha)) {}

EPuck_Environment_Classification::CollectedData::CollectedData() :
	count (1) {}

EPuck_Environment_Classification::Opinion::Opinion() :
	countedCellOfCurrentOpinion (0)  {}

EPuck_Environment_Classification::Movement::Movement() :
    walkTime (3),
    currentDirection (0) {}

/************************************* INIT **************************************************/
void EPuck_Environment_Classification::SimulationParameters::Init(TConfigurationNode& t_node) {

	try{
		/*
		Getting sigma, G value and the decision rule to follow (from config)
		 */
		GetNodeAttribute(t_node, "g", g);
		GetNodeAttribute(t_node, "sigma", sigma);
		GetNodeAttribute(t_node, "lambda", LAMBDA);
        GetNodeAttribute(t_node, "turn", turn);
		GetNodeAttribute(t_node, "decision_rule", decisionRule);
		GetNodeAttribute(t_node, "percent_red", percentRed);
		GetNodeAttribute(t_node, "percent_blue", percentBlue);
		GetNodeAttribute(t_node, "num_pack_saved", maxNumberOpinionStored);
	}
	catch(CARGoSException& ex) {
		THROW_ARGOSEXCEPTION_NESTED("Error initializing controller state parameters.", ex);
	}
}

void EPuck_Environment_Classification::Init(TConfigurationNode& t_node) {

	/* Initialize the actuators (and sensors) and the initial velocity as straight walking*/
	m_pcWheels = GetActuator<CCI_EPuckWheelsActuator>("epuck_wheels");
	m_pcProximity = GetSensor <CCI_EPuckProximitySensor >("epuck_proximity");
	m_pcLEDs = GetActuator<CCI_LEDsActuator>("leds");
	m_pcRABA = GetActuator<CCI_EPuckRangeAndBearingActuator>("epuck_range_and_bearing");
	m_pcRABS = GetSensor  <CCI_EPuckRangeAndBearingSensor>("epuck_range_and_bearing");
	m_pcRNG = CRandom::CreateRNG("argos");
	m_cGoStraightAngleRange.Set(-ToRadians(m_cAlpha), ToRadians(m_cAlpha));
	GetNodeAttributeOrDefault(t_node, "velocity", m_fWheelVelocity, m_fWheelVelocity);
	simulationParameters.Init(GetNode(t_node, "simulation_parameters"));

	simulationParameters.g = simulationParameters.g * SCALE;
	simulationParameters.sigma = simulationParameters.sigma * SCALE;

	/* Colours read from robots could be changed and added here */
	red.Set(COLOR_STRENGHT, 0, 0, ALPHA_CHANNEL);
	green.Set(0, COLOR_STRENGHT, 0, ALPHA_CHANNEL);
	blue.Set(0, 0, COLOR_STRENGHT, ALPHA_CHANNEL);

	/* Assign the initial state of the robots: all in exploration state */
	m_sStateData.State = SStateData::STATE_EXPLORING;

	std::string m_strOutput;
	m_strOutput = GetId();

	/* IC it's an helping variable to read others opinion. DEFAULT_VALUE_REB(5) is a default value */
	IC.receivedOpinion = DEFAULT_VALUE_REB;
	IC.receivedQuality = DEFAULT_VALUE_REB;
	IC.senderID = DEFAULT_VALUE_REB;

	/* INITIAL QUALITY: has to be estimated in the first exploration state */
	opinion.quality = 0;

	/* Init REB actuators*/
	CCI_EPuckRangeAndBearingActuator::TData toSend;
	toSend[0] = DEFAULT_VALUE_REB;
	toSend[1] = DEFAULT_VALUE_REB;
	toSend[2] = DEFAULT_VALUE_REB;
	toSend[3] = DEFAULT_VALUE_REB;
	m_pcRABA -> SetData(toSend);

    bool blueIsMajority = simulationParameters.percentRed < simulationParameters.percentBlue

    /* We want to study the case in which RED is the majority */
	if blueIsMajority
		simulationParameters.percentRed=simulationParameters.percentBlue;

	simulationParameters.percentRed=simulationParameters.percentRed / 100;
}

/*********************************** CONTROL STEP ****************************************/
void EPuck_Environment_Classification::ControlStep() {

	/* Turn leds according with currentOpinion and m_sStateDate.state */
	TurnLeds();

	/* Move robots following randomWalk */
	Move();

	/* Two different behaviours, depending on if they are diffusing or exploring */
	switch(m_sStateData.State) {

        case SStateData::STATE_EXPLORING: {
            Exploring();
            break;
        }

        case SStateData::STATE_DIFFUSING: {
            Diffusing();
            break;
        }
	}

	RandomWalk();

    ObstacleAvoidance();
}


void EPuck_Environment_Classification::ObstacleAvoidance() {

	/**** OBSTACLE AVOIDANCE (don't touch) ****/
	/* Get readings from proximity sensor and sum them together */
	const CCI_EPuckProximitySensor::TReadings& tProxReads = m_pcProximity -> GetReadings();

	CVector2 cAccumulator;
	for(size_t i = 0; i < tProxReads.size(); ++i) {
		cAccumulator += CVector2(tProxReads[i].Value, tProxReads[i].Angle);
	}

	if(tProxReads.size() > 0)
		cAccumulator /= tProxReads.size();

	/* If the angle of the vector is not small enough or the closest obstacle is not far enough curve a little */
	CRadians cAngle = cAccumulator.Angle();

	angleInRange = m_cGoStraightAngleRange.WithinMinBoundIncludedMaxBoundIncluded(cAngle)
    objectIsClose = cAccumulator.Length() < m_fDelta

	if(!(angleInRange && objectIsClose)) {

		if(cAngle.GetValue() > 0.0f) {
			m_pcWheels->SetLinearVelocity(m_fWheelVelocity, 0.0f);
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
	if ( movement.walkTime == 0 )                             // walkTime finished -> change direction
	{
		if ( movement.currentDirection == 0 )                  // If robot was going straight then turn standing in ->
			                                                  // -> a position for an uniformly distribuited time.
			                                                  // TODO: check directions!!
		{
			CRange<Real> zeroOne(0.0, 1.0);
			CRange<Real> minusOnePlusOne(-1.0, 1.0);

			Real p = m_pcRNG->Uniform(zeroOne);
			p = p * simulationParameters.turn;
			Real dir = m_pcRNG->Uniform(minusOnePlusOne);

			if ( dir > 0 )
				movement.currentDirection = 1;
			else
				movement.currentDirection = 2;
			movement.walkTime = Floor(p);
		}

		else 						// The robot was turning, time to go straight for an exponential period of time
		{
			movement.walkTime = Ceil(m_pcRNG->Exponential((Real)simulationParameters.LAMBDA) * 4); // *4: scaling factor, empiric
			movement.currentDirection = 0;
		}
	}
	else {							// The period of time is not finished, decrement the walkTime and keep the same direction
	    movement.walkTime--;
	}
}


/************************************************* EXPLORING STATE *********************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::Exploring() {

    m_pcRABS->ClearPackets();


	/* remainingExplorationTime it's the variable decremented each control step.
	 * This variable represents the time that a robot must still spend in exploration state.
	 * If this variable it's greater than zero, then it must be decremented and the robot should 
	 * do exploration's tasks
	 */
    explorationTimeLeft = (m_sStateData.remainingExplorationTime > 0)

	if(explorationTimeLeft)
	    m_sStateData.remainingExplorationTime--;


	/* If its time to change state, then the robot has to reset his own variables:
	 * - Assign a new random exponential time: remainingExplorationTime and totalExplorationDurationTime (used to
	 *   keep trace of the exploration times, just for statistic aims);
	 * - Calculate the quality of the opinion, basing on the sensed datas (Number of counted cells of currentf
	 *   opinion / Number of total counted cells);
	 * - Reset counting variables (countedCellOfcurrentOpinion and count [total number of cells counted]);
	 * - Change state: Exploration->Diffusing;
	 * - Generate a new Diffusing time (same as exploring, but used for Diffusing state and calculated with
	 *   different params for the random variable;
	 */
	else{ 
		 opinion.quality = (Real)opinion.countedCellOfCurrentOpinion / (Real)collectedData.count;
		 opinion.countedCellOfCurrentOpinion = 0;
		 receivedOpinions.clear();
		 collectedData.count = 1;
		 m_sStateData.State = SStateData::STATE_DIFFUSING;

		 /* Assigning a new exploration time, for the next exploration state */
		 m_sStateData.remainingExplorationTime = Ceil(m_pcRNG->Exponential((Real)simulationParameters.sigma));
		 m_sStateData.totalExplorationDurationTime = m_sStateData.remainingExplorationTime;

		 /*
		  * Assigning a new diffusing time for the incoming diffusing time, if the decision rule is the not-weighted
		  * direct comparison then the next diffusing time is weighted with the ideal quality of the best opinion
		  * [30: empirically calculated constant]
		  */
		 lambdaModulationPositiveFeedback = (Real)simulationParameters.g * (Real)opinion.quality
		 m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential(lambdaModulationPositiveFeedback) + LISTENING_TIME);
		 NWDirectComparison = simulationParameters.decisionRule==0
		 if(NWDirectComparison){
		    lambdaNotModulated = (Real)simulationParameters.g * (Real)simulationParameters.percentRed
			m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential(lambdaNotModulated) + LISTENING_TIME);
		 }
		 m_sStateData.totalDiffusingDurationTime = m_sStateData.remainingDiffusingTime;
	}
}

/************************************************* DIFFUSING STATE *********************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::Diffusing() {

    diffusingTimeLeft = m_sStateData.remainingDiffusingTime > 0
	if (diffusingTimeLeft)
	{
		/*
		 * In the 3 lasts seconds (30 ticks) the robot starts listening other opinions
		 * and diffusing his own opinion, quality and ID
		 */
		listeningTime =   m_sStateData.remainingDiffusingTime < LISTENING_TIME
		if (listeningTime)
		{
			/*
			 * Every received data is stored in IC variable (helping var). Each IC variable will be
			 * inserted in receivedOpinions array if has not been sensed yet and it's not a 0,0,0 one.
			 * It will be used to choose the next opinion, basing on decision rules. After a decision
			 * has been taken this array will be emptied for the next diffusing state.
			 */
			const CCI_EPuckRangeAndBearingSensor::TPackets& tPackets = m_pcRABS->GetPackets();

			for(size_t i = 0; i < tPackets.size() ; ++i) {
				bool saved = false; // saved == true -> don't save the datas

				/*
				 * IC = Helping variable for sensed opinions, if the received opinion is DEFAULT_VALUE_REB(5) then
				 * not save it DEFAULT_VALUE_REB
				 */
				IC.receivedOpinion = tPackets[i]->Data[0];
				nothingNewSensed = IC.receivedOpinion == DEFAULT_VALUE_REB
				if (nothingNewSensed)
					saved = true;

				IC.senderID = tPackets[i]->Data[3];

				/* Loop for sense quality value: quality has been sent using 3 cells of RAB datas,
				so here it will converted in a Real number */
				IC.receivedQuality=0;
				for ( UInt32 j = 1; j<N_COLORS ; ++j)
					IC.receivedQuality = IC.receivedQuality * 100 + tPackets[i]->Data[j];
				IC.receivedQuality = (Real) IC.receivedQuality / 10000;

				/* If the incoming value has already been listened then not save it */
				for(UInt32 j = 0; j < receivedOpinions.size(); ++j)
					if(receivedOpinions[j].senderID == IC.senderID)
						saved = true;

				/*
				 * 0,0,0 Values is one of the initializing values. We don't want to save it
				 */
				if((IC.senderID == 0) && (IC.receivedQuality==0) && (IC.receivedOpinion==0))
					saved = true;

				/* Save value if it has not been already saved and it's not 5,5,5 or 0,0,0 value  */
				if(!saved) {
					receivedOpinions.push_back(IC);
				}

			}
		}

		if(m_sStateData.remainingDiffusingTime%3)
			m_pcLEDs->SetAllColors(CColor::BLACK); // Blink leds

		/* Following things will have always to be done if in diffusing state. Here robot sends its opinion,
		 * quality and ID. toSend it's the variable of TData type used from Epuck RAB to send datas. We will
		 * prepare this variable before to send it. It has 4 Byte of datas
		 */
		CCI_EPuckRangeAndBearingActuator::TData toSend;

		/* First Byte used for the opinion of the robot */
		toSend[0] = opinion.currentOpinion;

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
			idConversion = (idConversion * 10) + (id[3]-'0');

		toSend[3] = idConversion;

		/* Send datas and decrement the diffusing time left */
		m_pcRABA->SetData(toSend);
		m_sStateData.remainingDiffusingTime--;
	}
	else // Time to change to exploration state
	{
		/* Direct comparison without weighted diffusing time */
		NWDirectComparison = simulationParameters.decisionRule==0
		if(NWDirectComparison)
		    lambdaNotModulated = (Real)simulationParameters.g * (Real)simulationParameters.percentRed
			m_sStateData.remainingDiffusingTime = Ceil(m_pcRNG->Exponential(lambdaNotModulated) + LISTENING_TIME);


		/* Change to EXPLORING state and choose another opinion with decision rules */
		m_sStateData.State = SStateData::STATE_EXPLORING;
		ApplyDecisionRule(simulationParameters.decisionRule);

		/* After decision has been taken, sensed values are deleted */
		receivedOpinions.clear();

		CCI_EPuckRangeAndBearingActuator::TData toSend;
		toSend[0]=DEFAULT_VALUE_REB;
		toSend[1]=DEFAULT_VALUE_REB;
		toSend[2]=DEFAULT_VALUE_REB;
		toSend[3]=DEFAULT_VALUE_REB;
		m_pcRABA->SetData(toSend);
	}
}

/* DECISION RULE */
void EPuck_Environment_Classification::ApplyDecisionRule(UInt32 decisionRule)
{

	switch(decisionRule) {

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

void EPuck_Environment_Classification::NotWeightedDirectComparison(){

	size_t size = receivedOpinions.size();
	std::vector<informationCollected> opinionsEvaluated;  // Set of information collected in every diffusing states

	if(receivedOpinions.size()>simulationParameters.maxNumberOpinionStored){
		for(size_t j=0; j<simulationParameters.maxNumberOpinionStored; j++){
			opinionsEvaluated.push_back(receivedOpinions[size-1-j]);
		}
	}
	else
	    for(size_t j=0; j<receivedOpinions.size(); j++)
		    opinionsEvaluated.push_back(receivedOpinions[j]);

	size = opinionsEvaluated.size();
	if(size > 0){
		CRange<Real> sizeRange(0,size);
		UInt32 index = (UInt32)Floor(m_pcRNG->Uniform(sizeRange));
		if ( opinionsEvaluated[index].receivedQuality > opinion.quality )
			opinion.currentOpinion = opinionsEvaluated[index].receivedOpinion;
	}
}

/* Randomly trust in one sensed opinion: receivedOpinions it's an array containing all the sensed opinions
 * relatively to the last diffusing state listening. With VoterModel, a robot randomly choose an opinion 
 * and uses it. 
 */
void EPuck_Environment_Classification::VoterModel(){

	size_t size = receivedOpinions.size();
	std::vector<informationCollected> opinionsEvaluated;  // Set of information collected in every diffusing states

	if(receivedOpinions.size()>simulationParameters.maxNumberOpinionStored){
		for(size_t j=0; j<simulationParameters.maxNumberOpinionStored; j++){
			opinionsEvaluated.push_back(receivedOpinions[size-1-j]);
		}
	}
	else
	    for(size_t j=0; j<receivedOpinions.size(); j++)
		    opinionsEvaluated.push_back(receivedOpinions[j]);

	size = opinionsEvaluated.size();

	if(size > 0){
		CRange<Real> sizeRange(0,size);
		UInt32 index = (UInt32)Floor(m_pcRNG->Uniform(sizeRange));
		opinion.currentOpinion = opinionsEvaluated[index].receivedOpinion;
	} 
}

/*
 * Randomly trust in one sensed opinion: receivedOpinions it's an array containing all the sensed opinions
 * relatively to the last diffusing state listening. With DirecComparison, a robot randomly choose an opinion 
 * and compare this opinion with his previous one. If it's stronger he uses this opinion. 
 */
void EPuck_Environment_Classification::DirectComparison(){

	size_t size = receivedOpinions.size();
	std::vector<informationCollected> opinionsEvaluated;  // Set of information collected in every diffusing states

	if(receivedOpinions.size()>simulationParameters.maxNumberOpinionStored){
		for(size_t j=0; j<simulationParameters.maxNumberOpinionStored; j++){
			opinionsEvaluated.push_back(receivedOpinions[size-1-j]);
		}
	}

	else
	    for(size_t j=0; j<receivedOpinions.size(); j++)
		    opinionsEvaluated.push_back(receivedOpinions[j]);
	size = opinionsEvaluated.size();
	if(size > 0){
		CRange<Real> sizeRange(0,size);
		UInt32 index = (UInt32)Floor(m_pcRNG->Uniform(sizeRange));
		if (opinionsEvaluated[index].receivedQuality > opinion.quality)
			opinion.currentOpinion = opinionsEvaluated[index].receivedOpinion;
	}
}

/*
 * Randomly trust in one sensed opinion: receivedOpinions it's an array containing all the sensed opinions
 * relatively to the last diffusing state listening. With MajorityRule, a robot counts the opinions and uses
 * the more present one. 
 */
void EPuck_Environment_Classification::MajorityRule(){

	UInt32 numberOpinionsReceived[N_COLORS];
	std::vector<informationCollected> opinionsEvaluated;  // Set of information collected in every diffusing states
	IC.receivedOpinion = opinion.currentOpinion;

	size_t size = receivedOpinions.size();
	if(receivedOpinions.size()>simulationParameters.maxNumberOpinionStored){
		for(size_t j=0; j<simulationParameters.maxNumberOpinionStored; j++){
			opinionsEvaluated.push_back(receivedOpinions[size-1-j]);
		}
	}
	else
	    for(size_t j=0; j<receivedOpinions.size(); j++)
		    opinionsEvaluated.push_back(receivedOpinions[j]);

	opinionsEvaluated.push_back(IC);

	/* Setting majority array to 0 */
	for ( UInt32 c = 0; c < N_COLORS; c++ )
		numberOpinionsReceived[c] = 0;

	/* For each received opinion, increment the correspondent cell. numberOpinionsReceived it's simply a contator for each color */
	for ( size_t i = 0; i<opinionsEvaluated.size(); i++ )
		numberOpinionsReceived[opinionsEvaluated[i].receivedOpinion]++;

	for( UInt32 i = 0; i<N_COLORS; i++)
		opinion.currentOpinion = FindMaxOpinionReceived(numberOpinionsReceived, opinion.currentOpinion);
}


UInt32 EPuck_Environment_Classification::FindMaxOpinionReceived(UInt32 numberOpinionsReceived[], UInt32 currentOpinion){

	UInt32 max = 0, index = 0;

	for( UInt32 i = 0; i<N_COLORS; i++)
		if( numberOpinionsReceived[i] > max )
		{
			max = numberOpinionsReceived[i];
			index = i;
		}
	if(max == 0)
		return currentOpinion;
	else
		return index;
}

/************************************************* MOVEMENT ****************************************************/
/***************************************************************************************************************/
void EPuck_Environment_Classification::Move(){
	switch(movement.currentDirection) {

        case 0: { // Straight
		    m_pcWheels->SetLinearVelocity(m_fWheelVelocity,  m_fWheelVelocity);
            break;
        }

        case 1: { // Left
			m_pcWheels->SetLinearVelocity(m_fWheelVelocity,  -m_fWheelVelocity);
            break;
        }

        case 2: { // Right
			m_pcWheels->SetLinearVelocity(-m_fWheelVelocity,  m_fWheelVelocity);
            break;
        }
	}
}

/************************************************* TURNING LEDS ON *********************************************/
/***************************************************************************************************************
0 = BLACK
1 = GREEN; 
2 = WHITE
*/
void EPuck_Environment_Classification::TurnLeds(){

	switch(opinion.currentOpinion) {

        case 0: {
            opinion.colorCurrentOpinion = CColor::BLACK;
            m_pcLEDs->SetAllColors(CColor::BLUE);
            break;
        }

        case 1: {
            opinion.colorCurrentOpinion = CColor::GREEN;   // Won't use for experiments
            m_pcLEDs->SetAllColors(CColor::GREEN);         // Won't use for experiments
            break;
        }

        case 2: {
            opinion.colorCurrentOpinion = CColor::WHITE;
            m_pcLEDs->SetAllColors(CColor::RED);
            break;
        }
	}
}

/****************************************/
/****************************************/
REGISTER_CONTROLLER(EPuck_Environment_Classification, "epuck_environment_classification_controller")



