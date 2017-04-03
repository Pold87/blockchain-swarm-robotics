pragma solidity ^0.4.8;

contract CollectiveDecisionSimilar {
    
    // TODO: NEVER RETURN 0 !!! (Since this is used as the default return value)
    // Colors are represented by a uint c, for example:
    // c == 0: white
    // c == 1: black
    
    uint numColors = 2;
    
    uint public wVotes;
    uint public bVotes;
    
    // Addresses of voter for option 0 and 1
    address[] c0;
    address[] c1;
    address[] addresses;
    
    // Array that collects how often a certain color has been voted for
    uint[] vote_counts;
    uint[] votes;
    uint[] qualities;


    // Defines a new type with two fields.
    struct Last2 {
        uint first;
        uint second;
    }
    
    function CollectiveDecisionSimilar() {
        // Initialize voting array
        vote_counts.length = numColors;
        addresses.length = 0;
        c0.length = 0;
        c1.length = 0;
        
        wVotes = 0;
        bVotes = 0;
        
    }
    
    
    function min(uint a, uint b) constant returns (uint) {
        if (a < b) {
            return a;
        } else {
            return b;
        }
    }
    
    
    function whiteVotes() constant returns (uint) {
        return wVotes;
    }
    
    function blackVotes() constant returns (uint) {
        return bVotes;
    }

    // Return the last vote in the blockchain
    function returnLast() constant returns (uint) {

      if (votes.length > 0) {
	return votes[votes.length - 1];
      } else {
	return 3;
      }
    }
    
    // Return the second to last vote in the blockchain
    function returnSecondToLast() constant returns (uint) {
      if (votes.length > 1) {
	return votes[votes.length - 2];
      } else {
	return 3;
      }
    }
    
    function voterModel(uint current_opinion, uint num_pack_saved) constant returns (uint) {
      
      // If there are no votes yet
      if (votes.length == 0) {
          return current_opinion;
      }
      uint r_max = min(num_pack_saved, votes.length);
      uint r = uint(block.blockhash(block.number - 1)) % r_max + 1;
      
      return votes[votes.length - r];
      
    }
    
    
   function directModulationRule(uint current_opinion, uint current_quality, uint num_pack_saved) constant returns (uint) {


      // If there are no votes yet
      if (votes.length == 0) {
          return current_opinion;
      }
      uint r_max = min(num_pack_saved, votes.length);
      uint r = uint(block.blockhash(block.number - 1)) % r_max + 1;
      
      uint to_compare = qualities[qualities.length - r];
      if (to_compare > current_quality) {
          return votes[votes.length - r];
      } else {
          return current_opinion;
      }
    }
    
    function majorityRule(uint current_opinion, uint num_pack_saved) constant returns (uint) {
        uint whites = 0;
        uint blacks = 0;
            
        // Look at num_pack_saved last votes
        uint up = min(num_pack_saved, votes.length);
        uint i = 0;
        while (i < up) {
            // Only look at opinions of other robots
            if (addresses[addresses.length - i - 1] != msg.sender) {    
                if (votes[votes.length - i - 1] == 0) {
                    whites++;
                } else if (votes[votes.length - i - 1] == 1) {
                    blacks++;
                } else {
                    throw;
                }
            } else {
                // Increase the upper limit
                up = min(num_pack_saved + 1, votes.length);
            }
            i++;
        }
        
        // Add own opinion
        if (current_opinion == 0) {
            whites++;
        } else {
            blacks++;
        }
        
        // Apply majority voting
        if (whites > blacks) {
            return 0;
        } else if (blacks > whites) {
            return 1;
        } else {
            return current_opinion;
        }
        
    }
    
    // Use one of the decision-making strategies and return the robots new opinion
    // 0: Not Weighted Direct Comparison    
    // 1: Voter Model
    // 2: Direct Comparison
    // 3: Majority Rule
    function applyStrategy(uint decision_rule, uint current_opinion, uint current_quality, uint num_pack_saved) constant returns (uint) {
        
        if (decision_rule == 0) {
            throw;
        } else if (decision_rule == 1) {
            return voterModel(current_opinion, num_pack_saved);
        } else if (decision_rule == 2) {
            return directModulationRule(current_opinion, current_quality, num_pack_saved);
        } else if (decision_rule == 3) {
            return majorityRule(current_opinion, num_pack_saved);
        } else {
            throw;
        }
        
    }
    
    
    // Vote for color opinion c with strategy s; the quality of the opinion is expressed in the 
    // amount of ether sent to the function 
    function vote(uint c, uint s) payable {
        
        if (s == 2) {
            
            // Convert ether to integer (for representing quality)
            uint q = msg.value;
            
            voteHelper(c, q);
        } else if (s == 3) {
        
            // Sending an opinion costs 1 ETH
            if (msg.value < 1 ether)
                throw;
            
            // Majority voting does not need a quality, therefore, always 1 is sent
            voteHelper(c, 1);
            
        }
        
    
    }
    
    
    // Vote for color opinion c with quality q
    function voteHelper(uint c, uint q) {
    
        
        // Check that it is only voted for valid colors 
        if (c >= numColors)
          throw;
       
        vote_counts[c] = vote_counts[c] + 1;
        votes.length++;
        votes[votes.length - 1] = c;
        
        addresses.length++;
        addresses[addresses.length - 1] = msg.sender;
        
        qualities.length++;
        qualities[qualities.length - 1] = q;
        

        // Save address of message sender
        if (c == 0) {
          //c0.length++;
          //c0[c0.length - 1] = msg.sender;
          wVotes++;
        } else {
          //c1.length++;    
          //c1[c1.length - 1] = msg.sender;
          bVotes++;
        }
        
   }
    
    
}
