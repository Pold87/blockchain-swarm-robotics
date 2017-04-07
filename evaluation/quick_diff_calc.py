from __future__ import division

import numpy as np

percentages = [34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48]

difficulties = []
for p in percentages:
    d = round(p / (100 - p), 3)
    difficulties.insert(len(difficulties), d)
    

# Create fake data for exit probability
with open("fake_python_exit.csv", 'w') as f:    
    f.write("difficulty,E.Ns,strategy\n")
    for s in ["Voter model", "Direct modulation", "Majority voting"]:
    #for s in ["Strat 1", "Strat 2", "Strat 3"]:
        en = round(np.random.normal(0.5, 0.05), 3)
        for d in difficulties:
            f.write(str(d) + ',' + str(en) + ',' + str(s) + '\n')
            en +=  round(np.random.normal(0.02, 0.01), 3)


Eas=range(20)
            
# Create fake data for consensus time
with open("fake_python_cons.csv", 'w') as f:    
    f.write("Ea0,consensus.time,strategy\n")
    for s in ["Voter model", "Direct modulation", "Majority voting"]:
    #for s in ["Strat 1", "Strat 2", "Strat 3"]:
        en = round(np.random.normal(200, 5), 0)
        for e in Eas:
            en -=  round(np.random.normal(5, 3), 3)
            if e == 0:
                f.write(str(e) + ',' + str(0) + ',' + str(s) + '\n')
            else: 
                f.write(str(e) + ',' + str(en) + ',' + str(s) + '\n')
            
        
        
        

