pragma solidity ^0.4.8;

contract Byzantine {

  enum Color {NONE, WHITE, BLACK}

  struct Opinion {
      Color color;
      uint quality;
  }

  struct votingInformation {
    Color opinion;
    uint blockNumber;
    bytes32 blockHash;
  }

  uint public wVotes;
  uint public bVotes;

  // For debugging
  uint constant firstBlock = 1150000;
  event print(uint v);
  event strategyApplied(address indexed sender, uint opinion, uint indexed blockNumber, bytes32 indexed blockHash);

    uint constant numColors = 2;
    uint constant contingent = 50;
    uint constant blockDelay = 6;
    uint constant k = 2; // Group size or max number of votes to consider
    bool useByzantineRobots = true;

    // Addresses of voter for option 0 and 1
    address[] c0;
    address[] c1;
    address[] addresses;
    // robotsOpinionPerBlock stores the opinion of all robots for all stable block
    // It allows to detect Byzantine robots
    mapping(address => mapping (uint => Color)) robotsOpinionPerBlock;
    //mapping(address => Color) robotsOpinionPerBlock;
    mapping(address => votingInformation) votingInformationPerRobot;
    mapping(uint => Opinion[]) votesPerBlock;
    mapping(address => bool) isByzantine;
    mapping(address => Color) lastOpinion;
    mapping(address => mapping (uint => uint)) robotsContingentPerBlock;

    // Constructor
    function Byzantine() {
      /*Opinion memory opiw = Opinion(Color.WHITE, 1);
      Opinion memory opib = Opinion(Color.BLACK, 1);
      votesPerBlock[block.number].push(opiw);
      votesPerBlock[block.number].push(opib);*/
    }

    function getBlock() returns (uint){
      return block.number;
    }

    function printVotesPerBlock() {

      // For the first 10 blocks
      for (uint j = 0; j < 100; j++){

        for (uint i = 0; i < votesPerBlock[firstBlock + j].length; i++) {
          Opinion storage o = votesPerBlock[firstBlock + j][i];
          print(uint(o.color));
      }
    }
  }

    // Private helper functions
    function min(uint a, uint b) private returns (uint) {
        return a < b ? a : b;
    }

    function max(uint a, uint b) private returns (uint) {
        return a > b ? a : b;
    }


    function maxInt(int a, int b) private returns (uint) {
      int res = a > b ? a : b;
      uint resUint = uint(res);
      return resUint;
    }

    function getVotesToConsider() private returns (Opinion[2]) {
      uint stableBlock = maxInt(int(block.number - blockDelay), int(0));
      uint r_max = votesPerBlock[stableBlock].length;
      uint r1 = (uint(block.blockhash(block.number - 1)) + uint(msg.sender)) % r_max;
      uint r2 = (block.timestamp + uint(msg.sender)) % r_max;
      Opinion memory v1 = votesPerBlock[stableBlock][r1];
      Opinion memory v2 = votesPerBlock[stableBlock][r2];

      return ([v1, v2]);
    }

    // Gets an array of all votes to consider and applies the voter model to them
    function voterModel(Opinion[2] opinionsToConsider) private constant returns (Color) {
      uint r = uint(block.blockhash(block.number - 1)) % k;
      return opinionsToConsider[r].color;
    }

    function directModulationRule(Opinion currentOpinion, Opinion[2] opinionsToConsider) private constant returns (Color) {
      uint r = (uint(block.blockhash(block.number - 1)) + uint(msg.sender)) % k;
      if (opinionsToConsider[r].quality > currentOpinion.quality) {
          return opinionsToConsider[r].color;
      } else {
          return currentOpinion.color;
      }
    }

    function majorityRule(Opinion currentOpinion, Opinion[2] opinionsToConsider) private constant returns (Color) {
      uint whites = 0;
      uint blacks = 0;

    // Count opinions to consider
    for (uint i = 0; i < k; i++) {
        if (opinionsToConsider[i].color == Color.WHITE) {
            whites++;
        } else if (opinionsToConsider[i].color == Color.BLACK) {
            blacks++;
        } else {
            revert();
        }
      }

      // Add own opinion
      if (currentOpinion.color == Color.WHITE) {
          whites++;
      } else if  (currentOpinion.color == Color.BLACK){
          blacks++;
      }

      // Apply majority voting
      if (whites > blacks) {
          return Color.WHITE;
      } else if (blacks > whites) {
          return Color.BLACK;
      } else {
          return currentOpinion.color;
      }
    }

    // Use one of the decision-making strategies and return the robots new opinion
    // 0: Not Weighted Direct Comparison
    // 1: Voter Model
    // 2: Direct Comparison
    // 3: Majority Rule

    function registerRobot(uint current_opinion) returns (uint, bytes32){
      uint stableBlock = maxInt(int(block.number - blockDelay), int(0));
      robotsOpinionPerBlock[msg.sender][stableBlock] = Color(current_opinion);
      lastOpinion[msg.sender] = Color(current_opinion);
      votingInformation memory vi = votingInformation(Color(current_opinion), stableBlock, block.blockhash(stableBlock));
      votingInformationPerRobot[msg.sender] = vi;
      strategyApplied(msg.sender, current_opinion, stableBlock, block.blockhash(stableBlock));
      robotsContingentPerBlock[msg.sender][stableBlock] = contingent;
      //robotsOpinionPerBlock[msg.sender] = Color(current_opinion);
      return (stableBlock, block.blockhash(stableBlock));
    }

    function getOpinionPerBlock(uint stableBlock) constant returns (uint, address) {
      return (uint(robotsOpinionPerBlock[msg.sender][stableBlock]), msg.sender);
      //return (uint(robotsOpinionPerBlock[msg.sender]), msg.sender);
    }

    function askForOpinion() constant returns (uint, uint, bytes32) {
      votingInformation vi = votingInformationPerRobot[msg.sender];
      return (uint(vi.opinion), vi.blockNumber, vi.blockHash);
    }

    function applyStrategy(uint decision_rule, uint current_opinion, uint current_quality)  returns (uint, uint, bytes32) {

        Color newOpinion;
        uint stableBlock = maxInt(int(block.number - blockDelay), int(0));

        votingInformation memory vi;

        if (votesPerBlock[stableBlock].length < 1) {
             //newOpinion = Color(current_opinion); // This might be a huuge bug!
             newOpinion = lastOpinion[msg.sender];
        } else {

        Opinion[2] memory votesToConsider = getVotesToConsider();
        Opinion memory opinionWithQuality = Opinion(Color(current_opinion), current_quality);

        // It might be a huuge problem, if robots are able to return their own opinion
        if (decision_rule == 0) {
            revert();
        } else if (decision_rule == 1) {
            newOpinion = voterModel(votesToConsider);
        } else if (decision_rule == 2) {
            newOpinion = directModulationRule(opinionWithQuality, votesToConsider);
        } else if (decision_rule == 3) {
            newOpinion = majorityRule(opinionWithQuality, votesToConsider);
        } else {
            revert();
        }
        }

        // Save opinion in the blockchain to be able to detect Byzantine robots
        robotsOpinionPerBlock[msg.sender][stableBlock] = newOpinion;
        lastOpinion[msg.sender] = newOpinion;
        vi = votingInformation(newOpinion, stableBlock, block.blockhash(stableBlock));
        votingInformationPerRobot[msg.sender] = vi;
        //robotsOpinionPerBlock[msg.sender] = newOpinion;
        strategyApplied(msg.sender, uint(newOpinion), stableBlock, block.blockhash(stableBlock));
        robotsContingentPerBlock[msg.sender][stableBlock] = contingent;
        // Return new opinion, block number six of six blocks ago, and block header
        return (uint(newOpinion), stableBlock, block.blockhash(stableBlock));
    }

    // Vote for color opinion c with strategy s; the quality of the opinion is expressed in the
    // amount of ether sent to the function
    function vote(uint c, uint s, uint stableBlock, bytes32 blockHash) payable returns (uint){


          // If the robot is Byzantine ignore its vote, i.e., don't do anything
          // Check if block number and blockhash correspond, i.e. "Do the robots have the same
          // blockchain version?"
          // The robot must send the opinions it's supposed to have
          // Check if the robot's opinion is too old
          uint minBlock = block.number - min(25, block.number);
          //if (block.blockhash(stableBlock) == blockHash && robotsOpinionPerBlock[msg.sender][stableBlock] == Color(c))  {

            if (stableBlock <= minBlock){
              return 8;
            }

            if (robotsOpinionPerBlock[msg.sender][stableBlock] == Color.NONE) {
              return 9;
            }

            if (block.blockhash(stableBlock) != blockHash)  {
              return 10;
            }

            if (robotsOpinionPerBlock[msg.sender][stableBlock] != Color.NONE && robotsOpinionPerBlock[msg.sender][stableBlock] != Color(c)) {
              isByzantine[msg.sender] = true;
              return 11;
            }

            if (robotsContingentPerBlock[msg.sender][stableBlock] == 0){
              return 12;
            }


          if (!isByzantine[msg.sender] && block.blockhash(stableBlock) == blockHash && robotsOpinionPerBlock[msg.sender][stableBlock] == Color(c) && (stableBlock > minBlock) && robotsContingentPerBlock[msg.sender][stableBlock] > 0)  {
          //if (!isByzantine[msg.sender] && block.blockhash(stableBlock) == blockHash && robotsOpinionPerBlock[msg.sender][stableBlock] == Color(c) && canStillUse[msg.sender][stableBlock])  {
          //  if (robotsOpinionPerBlock[msg.sender] == Color(c))  {

            Opinion memory o;

            // DC strategy
            if (s == 2) {

          // Ether represent quality
           o = Opinion(Color(c), msg.value);

          // DMVD or DMMD
            } else if (s == 1 || s == 3) {

            // Sending an opinion costs 1 wei
            if (msg.value < 1 wei)
                revert();

          // DMVD/DMMD do not need a quality, therefore, always 1 is sent
          o = Opinion(Color(c), 1);
        }

        if (uint(o.color) == 1) {
          wVotes++;
        } else if (uint(o.color) == 2) {
          bVotes++;
        }

        votesPerBlock[block.number].push(o);
        robotsContingentPerBlock[msg.sender][stableBlock] = robotsContingentPerBlock[msg.sender][stableBlock] - 1;
        return 1;
      }
      else {
        return 13;
      }

    }
}
