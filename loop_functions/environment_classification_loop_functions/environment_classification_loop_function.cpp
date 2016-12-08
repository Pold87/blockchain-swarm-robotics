#include "environment_classification_loop_function.h"
#include <argos3/core/utility/math/rng.h>

#define ALPHA_CHANNEL		         0
#define COLOR_STRENGHT               255
#define TOTAL_CELLS		             400
#define ENVIRONMENT_CELL_DIMENSION   1000.0f
#define N_COL					     3
#define ARENA_SIZE_X				 1.9f
#define ARENA_SIZE_Y				 1.9f
/****************************************/
/****************************************/
CEnvironmentClassificationLoopFunctions::CEnvironmentClassificationLoopFunctions() :

zeroOne(0.0f,1.0f),
bigRange(0.0f,30000.0f),
arenaSizeRangeX(0.0f, ARENA_SIZE_X),
arenaSizeRangeY(0.0f, ARENA_SIZE_Y),
m_bExperimentFinished(false),
m_pcFloor(NULL)
{
}


/************************************************* INIT ********************************************************/
/***************************************************************************************************************/
void CEnvironmentClassificationLoopFunctions::Init(TConfigurationNode& t_node) {


	incorrectParameters = false;
	m_pcRNG = CRandom::CreateRNG("argos");

	/* Setting variables according with the parameters of the configuration file (XML) */
	TConfigurationNode& tEnvironment = GetNode(t_node, "cells");
	try
	{
		/* Retrieving information about arena */
		GetNodeAttribute(tEnvironment, "number_of_red_cells", colorOfCell[0]);
		GetNodeAttribute(tEnvironment, "number_of_green_cells", colorOfCell[1]);
		GetNodeAttribute(tEnvironment, "number_of_blue_cells",colorOfCell[2]);
		GetNodeAttribute(tEnvironment, "percent_red", percentageOfColors[0]);
		GetNodeAttribute(tEnvironment, "percent_green", percentageOfColors[1]);
		GetNodeAttribute(tEnvironment, "percent_blue", percentageOfColors[2]);
		GetNodeAttribute(tEnvironment, "using_percentage", using_percentage);
		GetNodeAttribute(tEnvironment, "exit", exitFlag);

		/* Retrieving information about initial state of robots */
		GetNodeAttribute(tEnvironment, "r_0", initialOpinions[0]);
		GetNodeAttribute(tEnvironment, "g_0", initialOpinions[1]);
		GetNodeAttribute(tEnvironment, "b_0", initialOpinions[2]);
		GetNodeAttribute(tEnvironment, "number_of_robots", n_robots);
		GetNodeAttribute(tEnvironment, "number_of_qualities", number_of_qualities);

		/* Retrieving information about simulation paramaters */
		GetNodeAttribute(tEnvironment, "g", g);
		GetNodeAttribute(tEnvironment, "sigma", sigma);
		GetNodeAttribute(tEnvironment, "lamda", LAMDA);
		GetNodeAttribute(tEnvironment, "turn", turn);
		GetNodeAttribute(tEnvironment, "decision_rule", decisionRule);
		//		GetNodeAttribute(tEnvironment, "decision_rule", 1&); // TODO: Volker: change back 
		GetNodeAttribute(tEnvironment, "number_of_runs", number_of_runs);

		/* Retrieving information about how to catch and where to save statistics */
		GetNodeAttribute(tEnvironment, "save_every_ticks", timeStep);
		GetNodeAttribute(tEnvironment, "save_every_ticks_flag", everyTicksFileFlag);
		GetNodeAttribute(tEnvironment, "save_every_run_flag", runsFileFlag);
		GetNodeAttribute(tEnvironment, "save_every_quality_flag", qualityFileFlag);
		GetNodeAttribute(tEnvironment, "save_every_robot_flag", oneRobotFileFlag);
		GetNodeAttribute(tEnvironment, "save_global_stat_flag", globalStatFileFlag);
		GetNodeAttribute(tEnvironment, "radix", passedRadix);

	}
	catch(CARGoSException& ex) {
		THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
	}

	/* Translating percentages in numbers */
	if(using_percentage)
	{
		for( int c = 0; c < N_COL; c++ )
			colorOfCell[c] = (int)(percentageOfColors[c]*((Real)TOTAL_CELLS/100.0));
	}


	/*
	 * Check: number of robots = sum of the initial opinions
	 *        number of colors cells sums up to the total number of cells
	 */
	if((n_robots == initialOpinions[0]+initialOpinions[1]+initialOpinions[2])&&(colorOfCell[0]+colorOfCell[1]+colorOfCell[2] == TOTAL_CELLS))
	{
		consensousReached = N_COL;
		/* Multiply sigma and G per 10, so that they will be transformed into ticks (were inserted as  seconds) */
		sigma = sigma*10;
		g = g*10;
		max_time = max_time*10;

		/* Resetting number of opinions that have to be written */
		written_qualities = 0;
		int k = 0;
		/* Generate random color for each cell according with the choosen probabilities*/
		for ( int i = 0; i < N_COL; i++ )
			for( int j = 0; j < colorOfCell[i] ; j++,k++ )
				grid[k] = i;
		int temp;

		/* Mix the colours in the vector of cells to avoid the problem of eventual correlations*/
		for (int k = 0; k < 8; k++){
			for (int i = TOTAL_CELLS-1; i >= 0; --i){
				int j = ((int)m_pcRNG->Uniform(bigRange)%(i+1));
				temp = grid[i];
				grid[i] = grid[j];
				grid[j] = temp;
			}
		}

		/* Helper array, used to store and shuffle the initial opinions of the robots */
		UInt32 opinionsToAssign[n_robots];
		UInt32 i=0;

		m_pcRNG->SetSeed(std::clock());
		m_pcRNG->Reset();

		/* Build a vector containing the initial opinions */
		for (i=0; i<initialOpinions[0];i++)
			opinionsToAssign[i] = 0;

		for (i=initialOpinions[0] ;i<initialOpinions[0]+initialOpinions[1];i++)
			opinionsToAssign[i] = 1;

		for(i=initialOpinions[0]+initialOpinions[1]; i<initialOpinions[0]+initialOpinions[1]+initialOpinions[2];i++)
			opinionsToAssign[i] = 2;

		/* Vector shuffling, for randomize the opinions among the robots */
		for (int i = n_robots-1; i >= 0; --i){
			int j = (int)m_pcRNG->Uniform(bigRange) % (i+1);
			int temp = opinionsToAssign[i];
			opinionsToAssign[i] = opinionsToAssign[j];
			opinionsToAssign[j] = temp;
		}

		i=0;
		/* Setting variables for statistics */
		totalCountedCells  = 0;
		totalExploringTime = 0;
		for( int c = 0; c < N_COL; c++ )
		{
			/* Each position of the vectors corresponds to a different colour (Eg: Posiz 0 = RED, 1 = GREEN, 2 = BLUE */
			countedOpinions[c]      = 0;
			quality[c]              = 0;
			totalDiffusingTime[c]   = 0;
			numberOfExplorations[c] = 0;
		}

		i = 0;

		CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
		for(CSpace::TMapPerType::iterator it = m_cEpuck.begin();it != m_cEpuck.end();++it){
			/* Get handle to e-puck entity and controller */
			CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
			EPuck_Environment_Classification& cController =  dynamic_cast<EPuck_Environment_Classification&>(cEpuck.GetControllableEntity().GetController());
			EPuck_Environment_Classification::Opinion& opinion = cController.GetOpinion();
			EPuck_Environment_Classification::CollectedData& collectedData = cController.GetColData();

			/* Resetting initial state of the robots: exploring for everyone */
			cController.GetStateData().State = EPuck_Environment_Classification::SStateData::STATE_EXPLORING;
			Real tmpValue = (m_pcRNG->Exponential((Real)sigma));
			cController.GetStateData().remainingExplorationTime = tmpValue;
			cController.GetStateData().explorDurationTime =  cController.GetStateData().remainingExplorationTime;

			/* Assign a random actual opinion using the shuffled vector */
			opinion.actualOpinion = opinionsToAssign[i];
			i++;

			opinion.countedCellOfActualOpinion = 0;
			collectedData.count = 1;
			if(opinion.actualOpinion == 0)
				opinion.actualOpCol = CColor::RED;
			if(opinion.actualOpinion == 2)
				opinion.actualOpCol = CColor::BLUE;
			/* Setting robots initial states: exploring state */
		}
		std::string m_strOutput;

		/*
		 * File saving number of diffusing and exploring robots, for every opinion.
		 * It saves situation every timeStep ticks (timeStep/10 seconds)
		 */
		if(everyTicksFileFlag) {
			std::stringstream ss;
			ss << number_of_runs;
			std::string nRuns = ss.str();
			m_strOutput = passedRadix +".RUN"+nRuns;
			everyTicksFile.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
			everyTicksFile << "clock\texploringRed\tdiffusingRed\texploringGreen\tdiffusingGreen\texploringBlue\tdiffusingBlue\t" << std::endl;
		}

		/*
		 * File saving the the exit time and the number of robots (per opinion) after every run has been executed
		 */
		if(runsFileFlag){
			m_strOutput = passedRadix+".RUNS";
			runsFile.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
			runsFile << "Runs\t\tExitTime\tReds\t\tGreens\t\tBlues" << std::endl;
		}

		/*
		 * File saving the quality and the opinion of every robots after every changing from exploration to diffusing state
		 * (the quality and the   actualOpinion are the definitive ones)
		 */
		if(qualityFileFlag){
			m_strOutput = passedRadix + ".qualitiesFile";
			everyQualityFile.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
			everyQualityFile << "Q\tOP" << std::endl;
		}

		/* File saving all the statistics (times, counted cells and qualities) after the whole experiment is finished */
		if(globalStatFileFlag){
			m_strOutput = passedRadix + ".globalStatistics";
			globalStatFile.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
			globalStatFile << "TEX\tTC\tTDR\tQR\tCR\tTDG\tQG\tCG\tTDB\tQB\tCB\t" << std::endl;
		}
		if(oneRobotFileFlag){
			m_strOutput = passedRadix + ".oneRobotFile";
			oneRobotFile.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
		}

		/* Incorrect parameters (Number of robots or initial colors) -> Terminate the execution without write files */
	}else
	{
		incorrectParameters = true;
		IsExperimentFinished();
	}
}

void CEnvironmentClassificationLoopFunctions::Reset() {

	written_qualities = 0;

	m_pcRNG->SetSeed((int)m_pcRNG->Uniform(bigRange));
	m_pcRNG->Reset();

	GetSpace().SetSimulationClock(0);
	consensousReached = N_COL;

	for(size_t i = 0; i<N_COL; i++){
		robotsInExplorationCounter[i] = 0;
		robotsInDiffusionCounter[i] = 0;
	}

	/* Shuffling GRID vector (cells colours) to redistribuite them */
	for (int k = 0; k < 8; k++){
		for (int i = TOTAL_CELLS-1; i >= 0; --i){
			int j = ((int)m_pcRNG->Uniform(bigRange)%(i+1));
			int temp = grid[i];
			grid[i] = grid[j];
			grid[j] = temp;
		}
	}

	/* Helper array, used to store and shuffle the initial opinions of the robots */
	int opinionsToAssign[n_robots];

	/* Build a vector containing the initial opinions */
	for (int i=0; i<initialOpinions[0];i++)
		opinionsToAssign[i] = 0;

	for (int i=initialOpinions[0] ;i<initialOpinions[0]+initialOpinions[1];i++)
		opinionsToAssign[i] = 1;

	for (int i=initialOpinions[0]+initialOpinions[1]; i<n_robots;i++)
		opinionsToAssign[i] = 2;

	/* Vector shuffling, for randomize the opinions among the robots */
	for (int i = n_robots-1; i >= 0; --i){
		int j = ((int)m_pcRNG->Uniform(bigRange) % (i+1));
		int temp = opinionsToAssign[i];
		opinionsToAssign[i] = opinionsToAssign[j];
		opinionsToAssign[j] = temp;
	}


	/* Variable t is used to check the vector with the mixed opinion to assign a new opinion to every robots*/
	int t = 0;
	CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
	for(CSpace::TMapPerType::iterator it = m_cEpuck.begin();it != m_cEpuck.end();++it) {
		CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
		EPuck_Environment_Classification& cController =  dynamic_cast<EPuck_Environment_Classification&>(cEpuck.GetControllableEntity().GetController());
		EPuck_Environment_Classification::Opinion& opinion = cController.GetOpinion();
		EPuck_Environment_Classification::CollectedData& collectedData = cController.GetColData();

		cController.GetReceivedOpinions().clear();
		opinion.countedCellOfActualOpinion = 0;
		collectedData.count = 1;
		/* Assign a random actual opinion using the shuffled vector */
		opinion.actualOpinion = opinionsToAssign[t];
		if(opinion.actualOpinion == 0)
			opinion.actualOpCol = CColor::BLACK;
		if(opinion.actualOpinion == 2)
			opinion.actualOpCol = CColor::WHITE;
//		opinion.actualOpCol == CColor::BLACK;
//		std::cout<<opinion.actualOpCol<<std::endl;
		t++;
		cController.fromLoopFunctionRes();
	}

	AssignNewStateAndPosition();


	/*robots*/
}
/************************************************* RESET *******************************************************/
/***************************************************************************************************************/


/* Move every robot away from the arena*/
void CEnvironmentClassificationLoopFunctions::MoveRobotsAwayFromArena(UInt32 opinionsToAssign[]) {

	/* t is the index for the opinionToAssign vector */
	int t=0;
	/* Move every robot away from the arena (cNewPosition = CVector3(150.0f, 15.0f, 0.1f): a casual
      position) and assign a new initial opinion (opinionToAssign[t]) */
	CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
	for(CSpace::TMapPerType::iterator it = m_cEpuck.begin();it != m_cEpuck.end();++it) {
		CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
		EPuck_Environment_Classification& cController =  dynamic_cast<EPuck_Environment_Classification&>(cEpuck.GetControllableEntity().GetController());
		CQuaternion cNewOrientation = cEpuck. GetEmbodiedEntity().GetOriginAnchor().Orientation;
		CVector3 cNewPosition = CVector3(1.0f, 1.0f, 0.1f);
		cEpuck.GetEmbodiedEntity().MoveTo(cNewPosition, cNewOrientation, false);
		EPuck_Environment_Classification::Opinion& opinion = cController.GetOpinion();
		EPuck_Environment_Classification::CollectedData& collectedData = cController.GetColData();

		cController.GetReceivedOpinions().clear();

		/* Assign a random actual opinion using the shuffled vector */
	}
}

/* Assign a new state and a new position to the robots */
void CEnvironmentClassificationLoopFunctions::AssignNewStateAndPosition() {

	CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
	for(CSpace::TMapPerType::iterator it = m_cEpuck.begin();it != m_cEpuck.end();++it)
	{
		CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);
		EPuck_Environment_Classification& cController =  dynamic_cast<EPuck_Environment_Classification&>(cEpuck.GetControllableEntity().GetController());
		CQuaternion cNewOrientation = cEpuck. GetEmbodiedEntity().GetOriginAnchor().Orientation;

		/* Generating Uniformly distribuited x and y coordinates for the new position of the robot */
		Real xp = m_pcRNG->Uniform(arenaSizeRangeX);
		Real yp = m_pcRNG->Uniform(arenaSizeRangeY);
		CVector3 cNewPosition = CVector3(xp,yp,0.1f);

		UInt32 i, unMaxRepositioningTrials = 100;

		for(i = 0; i < unMaxRepositioningTrials; i++) {
			if(cEpuck.GetEmbodiedEntity().MoveTo(cNewPosition, cNewOrientation, false)) break;

			xp = m_pcRNG->Uniform(arenaSizeRangeX);
			yp = m_pcRNG->Uniform(arenaSizeRangeY);

			cNewPosition.SetX(xp);
			cNewPosition.SetY(yp);
		}

		cController.GetReceivedOpinions().clear();
		/* Resetting initial state of the robots: exploring for everyone */
		cController.GetStateData().State = EPuck_Environment_Classification::SStateData::STATE_EXPLORING;
		cController.GetStateData().remainingExplorationTime = (m_pcRNG->Exponential((Real)sigma));
		cController.GetStateData().explorDurationTime =  cController.GetStateData().remainingExplorationTime;
	}
}

/************************************************ IS EXPERIMENT FINISHED ***************************************/
/***************************************************************************************************************/
bool CEnvironmentClassificationLoopFunctions::IsExperimentFinished() {

	/* If parameters are uncorrect then finish the experiment (Eg: number of robots vs sum of the initial opinion,
	 * or the colours of the cells mismatching */
	if( incorrectParameters )
		return true;

	/* Vary termination condition: [GetSpace().GetSimulationClock() >= max_time] [consensousReached != N_COL]
	 * [NumberOfQualities == WrittenQualities ] */
	CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
	CSpace::TMapPerType::iterator it = m_cEpuck.begin();
	if(exitFlag){
		if( consensousReached != N_COL ){ 
			number_of_runs--;

			/* RUNSFILE: Write statistics of the last run */
			if (runsFile.is_open()){
				runsFile << number_of_runs+1 <<"\t\t"
						<< (GetSpace().GetSimulationClock()-1)/10 <<"\t\t";

				for ( UInt32 c = 0; c < N_COL; c++ )
					runsFile << robotsInDiffusionCounter[c] + robotsInExplorationCounter[c] <<"\t\t";
				runsFile<<std::endl;
			}

			if (number_of_runs<=0) {

				/* Close runsFile*/
				if(runsFile.is_open())
				{
					runsFile.close();
				}
				/* Calcolate statistics */
				UInt32 totalNumberOfExplorations = 0;
				for ( UInt32 c = 0; c < N_COL; c++){
					totalNumberOfExplorations += numberOfExplorations[c];
					totalDiffusingTime[c] = totalDiffusingTime[c]/numberOfDiffusions[c];
					quality[c] = (Real)((quality[c])/((Real)numberOfExplorations[c]));
					countedOpinions[c] = (Real)((Real)(countedOpinions[c])/(Real)(numberOfExplorations[c]));
				}
				totalExploringTime = (Real)((Real)totalExploringTime/(Real)totalNumberOfExplorations);
				totalCountedCells=(Real)((Real)totalCountedCells/(Real)totalNumberOfExplorations);

				/* Close qualities file */
				if (everyQualityFile.is_open()){
					everyQualityFile.close();
				}

				/* globalStatFile: write the general statistics, such as counted cells,
		   times of diffusing and exploring */
				if (globalStatFile.is_open()){
					globalStatFile<< std::setprecision(3) << totalExploringTime/10 << "\t"
							<< std::setprecision(3) << totalCountedCells << "\t";
					for ( UInt32 c = 0; c < N_COL; c++){
						globalStatFile<< std::setprecision(3) << totalDiffusingTime[c]/10 << "\t"
								<< std::setprecision(3) << quality[c] << "\t"
								<< std::setprecision(3) << countedOpinions[c] << "\t";
					}
					globalStatFile.close();
				}

				/* Set experimentFinished variable to true -> the experiment will terminate */
				m_bExperimentFinished = true;
			}
			else  {
				written_qualities = 0;
				Reset();
			}
		}
		return m_bExperimentFinished;
	}
	else {

		if( written_qualities == number_of_qualities){ //consensousReached != N_COL){  written_qualities == number_of_qualities
			number_of_runs--;

			/* RUNSFILE: Write statistics of the last run */
			if (runsFile.is_open()){
				runsFile << number_of_runs+1 <<"\t\t"
						<< (GetSpace().GetSimulationClock()-1)/10 <<"\t\t";

				for ( UInt32 c = 0; c < N_COL; c++ )
					runsFile << robotsInDiffusionCounter[c] + robotsInExplorationCounter[c] <<"\t\t";
				runsFile<<std::endl;
			}

			if (number_of_runs<=0) {

				/* Close runsFile*/
				if(runsFile.is_open())
				{
					runsFile.close();
				}
				/* Calcolate statistics */
				UInt32 totalNumberOfExplorations = 0;
				for ( UInt32 c = 0; c < N_COL; c++){
					totalNumberOfExplorations += numberOfExplorations[c];
					totalDiffusingTime[c] = (Real)((Real)totalDiffusingTime[c]/(Real)numberOfDiffusions[c]);
					quality[c] = (Real)((quality[c])/((Real)numberOfExplorations[c]));
					countedOpinions[c] = (Real)((Real)(countedOpinions[c])/(Real)(numberOfExplorations[c]));
				}
				totalExploringTime = (Real)((Real)totalExploringTime/(Real)totalNumberOfExplorations);
				totalCountedCells=(Real)((Real)totalCountedCells/(Real)totalNumberOfExplorations);

				/* Close qualities file */
				if (everyQualityFile.is_open()){
					everyQualityFile.close();
				}

				/* globalStatFile: write the general statistics, such as counted cells,
		   times of diffusing and exploring */
				if (globalStatFile.is_open()){
					globalStatFile<< std::setprecision(3) << totalExploringTime/10 << "\t"
							<< std::setprecision(3) << totalCountedCells << "\t";
					for ( UInt32 c = 0; c < N_COL; c++){
						globalStatFile<< std::setprecision(3) << totalDiffusingTime[c]/10 << "\t"
								<< std::setprecision(3) << quality[c] << "\t"
								<< std::setprecision(3) << countedOpinions[c] << "\t";
					}
					globalStatFile.close();
				}

				/* Set experimentFinished variable to true -> the experiment will terminate */
				m_bExperimentFinished = true;
			}
			else  {
				written_qualities = 0;
				Reset();
			}
		}
		return m_bExperimentFinished;}
}

/************************************************* DESTROY *****************************************************/
/***************************************************************************************************************/
void CEnvironmentClassificationLoopFunctions::Destroy(){}

/************************************************ PRESTEP ******************************************************/
/***************************************************************************************************************/

void CEnvironmentClassificationLoopFunctions::PreStep() {

	/* Reset counters: these array are counting the number of robots in each state. Every position corresponds to a color:
	robotsInExplorationCounter[0] -> number of robots exploring with opinion red
	robotsInExplorationCounter[1] -> number of robots exploring with opinion green 
	... */

	for ( UInt32 c=0; c<N_COL; c++ ){
		robotsInExplorationCounter[c] = 0;
		robotsInDiffusionCounter[c] = 0;
	}
	CSpace::TMapPerType& m_cEpuck = GetSpace().GetEntitiesByType("epuck");
	for(CSpace::TMapPerType::iterator it = m_cEpuck.begin();it != m_cEpuck.end();++it){
		/* Get handle to e-puck entity and controller */
		CEPuckEntity& cEpuck = *any_cast<CEPuckEntity*>(it->second);

		EPuck_Environment_Classification& cController =  dynamic_cast<EPuck_Environment_Classification&>(cEpuck.GetControllableEntity().GetController());


		Real x = cEpuck. GetEmbodiedEntity().GetOriginAnchor().Position.GetX(); // X coordinate of the robot
		Real y = cEpuck. GetEmbodiedEntity().GetOriginAnchor().Position.GetY(); // Y coordinate of the robot

		CVector2 cPos;
		cPos.Set(x,y);						// Vector position of the robot
		/* Figure out in which cell (EG: which is the index of the array grid) the robot is */
		UInt32 cell = (UInt32) ((y+0.009)*10000)/(Real)ENVIRONMENT_CELL_DIMENSION;
		cell = (UInt32) 40*cell + ((x+0.009)*10000)/(Real)ENVIRONMENT_CELL_DIMENSION;

		/* Get parameters of the robot: color, state, opinion and movement datas*/
		EPuck_Environment_Classification::CollectedData& collectedData = cController.GetColData();
		EPuck_Environment_Classification::SStateData& sStateData = cController.GetStateData();
		EPuck_Environment_Classification::Movement& movement = cController.GetMovement();
		EPuck_Environment_Classification::Opinion& opinion = cController.GetOpinion();
		std::string id = cController.GetId();
		EPuck_Environment_Classification::SimulationState& simulationParam = cController.GetSimulationState();

		/* Update statistics about the robot opinions*/
		UpdateStatistics(opinion,sStateData);
		if(cController.IsExploring())
			UpdateCount(collectedData, cell, cPos, opinion, sStateData, id, simulationParam);
		RandomWalk(movement);
	}

	/* Check if a consensous has been reached (i.e.: if every robots agree with a colour) */
	for ( UInt32 c = 0; c < N_COL ; c++ )
		if( robotsInExplorationCounter[c] + robotsInDiffusionCounter[c] == n_robots )
			consensousReached = c;

	/* EVERYTICKSFILE: Write this statistics only if the file is open and it's the right timeStep (multiple of timeStep) */
	if ( ! (GetSpace().GetSimulationClock() % timeStep) )
		if (everyTicksFile.is_open())
		{
			everyTicksFile << (GetSpace().GetSimulationClock())/10 << "\t";
			for ( UInt32 c = 0; c < N_COL; c++ )
				everyTicksFile << robotsInExplorationCounter[c] << "\t\t" << robotsInDiffusionCounter[c]  << "\t\t";
			everyTicksFile << std::endl;
		}
	 everyTicksFile << (GetSpace().GetSimulationClock())/10 <<std::endl;
}


void CEnvironmentClassificationLoopFunctions::UpdateStatistics(EPuck_Environment_Classification::Opinion& opinion, EPuck_Environment_Classification::SStateData& sStateData) {

	/* Increment counters of the opinions and states of the robots */
	if(sStateData.State == EPuck_Environment_Classification::SStateData::STATE_EXPLORING)
		robotsInExplorationCounter[opinion.actualOpinion]++;
	if(sStateData.State == EPuck_Environment_Classification::SStateData::STATE_DIFFUSING)
		robotsInDiffusionCounter[opinion.actualOpinion]++;
}

/* Update count of the total number of cells and of the cells according with the opinion*/
void CEnvironmentClassificationLoopFunctions::UpdateCount(EPuck_Environment_Classification::CollectedData& collectedData, UInt32 cell, CVector2 cPos, EPuck_Environment_Classification::Opinion& opinion, EPuck_Environment_Classification::SStateData& sStateData, std::string& id, EPuck_Environment_Classification::SimulationState& simulationParam) {

	collectedData.readColor = GetFloorColor(cPos);

	if(collectedData.readColor == opinion.actualOpCol) 	       // If is as my opinion //
		opinion.countedCellOfActualOpinion++;                  // Increment opinion counter)//

	collectedData.count++;

    // However increment the count of cells passed thorugh //

	/* Collecting datas for statistics: times and qualities *///	
	if(sStateData.remainingExplorationTime == 1){

	   oneRobotFile<<std::setprecision(3) << opinion.quality<<" ";
	   written_qualities++;

	}
}
/* Implement random walk */
void CEnvironmentClassificationLoopFunctions::RandomWalk(EPuck_Environment_Classification::Movement& movement) {

	/* walkTime represents the number of clock cycles (random number) of walk in a random direction*/
	if ( movement.walkTime == 0 )                            // Is the walkTime in that direction finished? ->
	{ 				                                         // -> YES: change direction//

		if ( movement.actualDirection == 0 )                  // If robot was going straight then turn standing in ->
			                                                  // -> a position for an uniformly distribuited time //
		{
			Real p = m_pcRNG->Uniform(zeroOne);
			p = p*turn - (turn/2);
			if ( p > 0 )
				movement.actualDirection = 1;
			else
				movement.actualDirection = 2;
			movement.walkTime = (UInt32) abs(p);
		}

		else 						// The robot was turning, time to go straight for ->
			                        // -> an exponential period of time //
		{
			movement.walkTime = (m_pcRNG->Exponential((Real)LAMDA))*4; // Exponential random generator. *50 is a scale factor for the time
			movement.actualDirection = 0;
		}
	}
	else 							// NO: The period of time is not finished, decrement the ->
		                            // -> walkTime and keep the direction //
		movement.walkTime--;
} 


REGISTER_LOOP_FUNCTIONS(CEnvironmentClassificationLoopFunctions, "environment_classification_loop_functions")

