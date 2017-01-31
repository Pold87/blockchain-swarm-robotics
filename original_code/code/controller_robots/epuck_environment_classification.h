/** @file </EC_Cluster/controllers/epuck_environment_classification.h>
 *
 * This class is meant to be the header file of the controller of our E-Puck.
 * In this class we'll define the methods and variables used to define the robots'
 * behaviour during the experiment.
 *
 * @author Davide Brambilla - <davide14.brambilla@polimi.it>
 */

#ifndef EPUCK_ENVIRONMENT_CLASSIFICATION_H
#define EPUCK_ENVIRONMENT_CLASSIFICATION_H

#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_proximity_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_ground_sensor.h>
#include <argos3/plugins/robots/e-puck/control_interface/ci_epuck_wheels_actuator.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector2.h>

#define N_COLORS  3

/**
 * Header
 */
namespace argos{
 
class EPuck_Environment_Classification : public CCI_Controller {

    public:
          /** Data collected during the exploration state (used IR ground sensor) */
          struct CollectedData {
             CColor readColor;
             UInt32 count;
             CollectedData();
          };

          /** Information collected during the listening state */
          struct informationCollected{
             UInt8 receivedOpinion;
             Real receivedQuality;
             UInt8 senderID;
          };

          struct Opinion {
             UInt32 currentOpinion;                 // 'ID' of the opinions
             UInt32 countedCellOfCurrentOpinion;
             Real quality;
             CColor colorCurrentOpinion;
             Opinion();
          };

          struct Movement {
            SInt32 walkTime;
            UInt32 currentDirection;  // 0, straight; 1, turn CW; 2, turn Counter-CW
            Movement();
          };

          /** Struct used to store the parameters strictly related to the
           *  experiment's setup that have been set in the configuration file:
           *  (EC_Cluster/experiments/c_epuck_environment_classification.argos).
           *  For more details about g, sigma, LAMBDA and turn, refer to:
           *  (see: http://iridia.ulb.ac.be/IridiaTrSeries/link/IridiaTr2015-005.pdf)
           */
          struct SimulationParameters {
             Real percentRed, percentBlue;              // Difficulty of the problem: percentage of colors on the arena
             Real g, sigma;                             // Parameters for the exponential durations of the states
             UInt32 decisionRule;                       // Store the decision rule to be studied
             UInt32 maxNumberOpinionStored;             // Max. number of incoming messages that the robots will store
             UInt32 status;                             // Initial state
             UInt32 LAMBDA, turn;                       // Movement parameters
             std::string radix;                         // Radix of the file where to save
             void Init(TConfigurationNode& t_node);
          };

          /** Struct used to keep track of the duration of the states and switch state */
          struct SStateData {

             SInt32 remainingDiffusingTime;
             SInt32 remainingExplorationTime;
             SInt32 totalDiffusingDurationTime;
             SInt32 totalExplorationDurationTime;

             enum EState {
                 STATE_EXPLORING,
                 STATE_DIFFUSING,
             } State;
          };

       /** Class constructor and destructor. */
       EPuck_Environment_Classification();
       virtual ~EPuck_Environment_Classification() {}
       virtual void Destroy() {}

       /**  Set configuration parameters read from the config. file:
        *   (EC_Cluster/experiments/c_epuck_environment_classification.argos [xml]).
        */
       virtual void Init(TConfigurationNode& t_node);

       /**
        * Receives the index of the decision rule to apply and on that base calls the
        * appropriate function to apply that decision rule. All the functions apply a
        * specific decision rule on the set of opinion listened and, depending on
        * the result, they modify the current opinion of the robot.
        * For further explanation about the decision rules:
        * Refer to [ http://iridia.ulb.ac.be/IridiaTrSeries/link/IridiaTr2015-005.pdf ]
        * @param decisionRule the decision rule to apply
        * @return
        */
       void ApplyDecisionRule(UInt32 decisionRule);

       /**
        * Applies the Direct Comparison to the set of opinions
        */
       void DirectComparison();

       /**
        * Applies the Majority Rule to the set of opinions
        */
       void MajorityRule();

       /**
        * Applies the Not Weighted Direct Comparison to the set of opinions
        */
       void NotWeightedDirectComparison();

       /**
        * Applies the Voter Model to the set of opinions
        */
       void VoterModel();

       /**
        * This defines all the actions that have to be performed in one control step by the robot.
        *
        *    High level description:
        *    This function describes the behaviour defined by this strategy for the robot.
        *    In every time step, the robot moves randomly in the environment (activating the wheels
        *    of the robot according to the random walk's function). On top of that, the robot performs
        *    more specific actions depending by their current state (see Diffusing() and Exploring())
        *
        * This function calls the function Move(), in order to activate the wheels. Sequentially it
        * calls the function RandomWalk(), which modify certain global variables of the robot that
        * will randomly 'drive' the robot. Once modified the global variables,
        * @param
        * @return
        */
       virtual void ControlStep();

       /**
        * Defines the behaviour of the robots in diffusing state. In this state, the robot has to
        * broadcast their current opinion in a limited range through their Range and Bearing (IR) actuators.
        * 3 Seconds before the end of this time, the robot starts listening simultaneously to the broadcasting.
        * This state lasts for an exponential random time, that is set at the end of the exploring
        * state (Exploring()) and that depends by the quality estimated.
        * When the time to diffuse is over, the robots have stored the opinions of the neighbours
        * (min = 0 opinions, max = maxNumberOpinionStored opinions) and calls the ApplyDecisionRule function,
        * in order to compute the change of current opinion.
        * @param
        * @return
        */
       void Diffusing();

       /**
        * Defines the behaviour of the robots in exploring state. In this state, the robot has to
        * estimate the quality of its current opinion. To do so, it counts all the cells of the same color as its
        * current preference. To detect the color of the cells, the robot uses its ground sensor (IR sensors).
        * The quality is a normalized value to the total number of the cells 'saw' by the robot.
        * In this state is set the duration of the NEXT diffusing state by means of an exponentially distributed
        * random variable that uses the quality estimated as weighting factor for the exponential parameter.
        * @param
        * @return
        */
       void Exploring();

       /**
        * This function defines the last 3 Seconds of execution of each diffusing state. In this function the robot
        * uses its Range and Bearing (IR) sensors to sense the opinion of the neighbours, which stores in the internal
        * variables (struct: informationCollected). This function is called from internally the diffusing
        * state (Diffusing())
        */
       void Listening();

       /**  Perform the movements */
       void Move();
       virtual void RandomWalk();

       /**  Bring the robot to initial state  */
       virtual void Reset() {};

       /** Handle leds */
       void TurnLeds();

       /** Find the max value among the received ones */
       UInt32  FindMaxOpinionReceived(UInt32 numberOpinionsReceived[], UInt32 currentOpinion);

       Real ExponentialFormula(Real mean){
           CRange<Real> cRange(0.0,1.0);
           return -log(m_pcRNG->Uniform(cRange)) * mean;
       }

       /** Getter methods*/
       inline CollectedData& GetColData() {
          return collectedData;
       }
       inline SStateData& GetStateData() {
          return m_sStateData;
       }
       inline SimulationParameters& GetSimulationParameters(){
          return simulationParameters;
       }
       inline std::vector<informationCollected>& GetReceivedOpinions() {
          return receivedOpinions;
       }
       inline Movement & GetMovement() {
          return movement;
       }
       inline Opinion & GetOpinion() {
          return opinion;
       }
       inline bool IsExploring() const {
          return m_sStateData.State == SStateData::STATE_EXPLORING;
       }
       inline bool IsDiffusing() const {
          return m_sStateData.State == SStateData::STATE_DIFFUSING;
       }

    private:

       CCI_EPuckWheelsActuator* m_pcWheels;
       Real m_fWheelVelocity;
       CCI_EPuckRangeAndBearingActuator*  m_pcRABA; // Range and Bearing actuators
       CCI_EPuckRangeAndBearingSensor* m_pcRABS;    // Range and Bearing sensors

       CDegrees m_cAlpha;                           //
       Real m_fDelta;                               // Obstacle Avoidance
       CCI_EPuckProximitySensor* m_pcProximity;     //
       CRange<CRadians> m_cGoStraightAngleRange;    //

       CCI_LEDsActuator* m_pcLEDs;
       CRandom::CRNG* m_pcRNG;                      // random number generator

       /**Files */
       std::ofstream epuckFile;

       /** Data structures for collect opinions when in diffusing state */
       informationCollected IC;
       std::vector<informationCollected> receivedOpinions;

       /**All others used variables */
       SStateData m_sStateData;
       SimulationParameters simulationParameters;
       std::ofstream numberReceived;
       CollectedData collectedData;
       Opinion opinion;
       Movement movement;
       int initializationValues[N_COLORS];
       CColor red, blue, green;                    // Add here eventual additional colors
    };
}  // namespace argos

#endif // EPUCK_ENVIRONMENT_CLASSIFICATION_H