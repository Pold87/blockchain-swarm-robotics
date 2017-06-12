import sys

outfile="config0.txt"

num_robots=20
#miner_id=111

used_nodes = [0, 2]

# Threshold; all robot ids below this value will be run on node 1, the
# rest on node 2
t1 = 10

# For now this script only works with 20 robots;
# I'll have to write it for 100 robots

with open(outfile, 'w') as f:
    for i in xrange(num_robots):
        if (i < t1):
            node = used_nodes[0]
        else:
            node = used_nodes[1]
        
        f.write(str(i) + ' ' + str(node) + '\n')

    #f.write(str(miner_id) + ' ' + str(used_nodes[1]))
            
print("Created node mapping")    
