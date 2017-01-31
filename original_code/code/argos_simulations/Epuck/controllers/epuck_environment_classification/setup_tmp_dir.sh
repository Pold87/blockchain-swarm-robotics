#!/bin/bash
USERNAME=`whoami`

for node in c1-1 c1-2 c1-3 c1-4 c1-5 c1-6 c1-7 c1-8 c1-9 c1-10 c1-11 c1-12 c1-13 c1-14 c1-15 c2-0 c2-1 c2-2 c2-3 c2-4 c2-5 c2-6 c2-7 c2-8 c2-9 c2-10 c2-11 c2-12 c2-13 c2-14 c2-15 c2-16 c2-17 c2-18 c2-19 c2-20 c2-21 c2-22 c2-23 c2-24 c2-25 c2-26 c2-27 c2-28 c2-29 c2-30 c2-31 c3-0 c3-1 c3-2 c3-3 c3-4 c3-5 c3-6 c3-7 c3-8 c3-9 c3-10 c3-11 c3-12 c3-13 c3-14 c3-15 c4-0 c4-1 c4-2 c4-3 c4-4 c4-5 c4-6 c4-7 c4-8 c4-9 c4-10 c4-11 c4-12 c4-13 c4-14 c4-15 
do
  echo "Working on node $node"
  ssh $node "rm -rf /tmp/$USERNAME" &> /dev/null
  ssh $node "mkdir /tmp/$USERNAME" &> /dev/null
#  scp -r /lustre/home/dbrambilla/svn/ArgosSimulation/ $node:/tmp/$USERNAME
#  scp -r /lustre/home/dbrambilla/argos3-dist $node:/tmp/$USERNAME
done
