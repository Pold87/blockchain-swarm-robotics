# Managing Byzantine Robots via Blockchain Technology in a Swarm Robotics Collective Decision Making Scenario


This repository contains the code for the paper:

V.Strobel, E. Castello Ferrer, M. Dorigo. 2018.
Managing Byzantine Robots via Blockchain Technology in a Swarm Robotics Collective Decision Making Scenario.

*To appear in Proceedings of the 17th Conference on Autonomous Agents and MultiAgent Systems. International Foundation for Autonomous Agents and Multiagent Systems (AAMAS 2018).*

A pre-print is available at:  
http://iridia.ulb.ac.be/IridiaTrSeries/

## Instructions

There is a .ova virtual machine file that contains a runnable version of this repository.
The link to the VM image is https://drive.google.com/file/d/1QGTEAIyoeGffcfBEzSbq4xbTcq8bLe7S/view?usp=sharing (the files are in `~/blockchain-swarm-robotics/`).

User: Iridia  
pw: Iridia

Execute Experiment 1 with:

`bash start_experiment1.sh 0 0 1`

Please note that we are currently working on a platform-independent
version of this repository (currently, it contains the code for our
architecture but there is a more versatile version in the branch
'local'). 
Some hints for the installation (more will follow soon):



### Requirements:
- ARGoS 3
- ARGoS-epuck
- git
- cmake
- golang (tested with version 1.7.3)
- optional: sendmail

The easiest way to install ARGoS and the epuck plugin is via the
script `install_argos.sh` (install the dependencies mentioned at https://github.com/ilpincy/argos3 first.

Then, execute `create_geths.sh` and add the created geths (default is
just `geth0`) to your PATH (incl. export).

Install the solidity compiler `solc`.

Edit the file `start_experiment1.sh` (change the variable BASE etc.).

Compile the code (mkdir build; cd build; cmake ..; make).

Run an experiment: `bash start_experiment1.sh 0 0 1`


## Scenario

Using the robot swarm simulator ARGoS 3, we study a collective
decision scenario, in which robots sense which of two features in an
environment is the most frequent one---a best-of-2 problem. Our
approach is based on the collective decision scenario of Valentini et
al. Via blockchain-based smart contracts using the Ethereum protocol,
we add a security layer on top of the classical approach that allows
to take care of the presence of Byzantine robots. Our blockchain
approach also allows to log events in a tamper-proof way: these logs
can then be used to analyze, if necessary, the behavior of the robots
in the swarm without incurring the risk that some malicious agent has
modified them. In addition, it provides a new way to understand how we
debug and do data forensics on decentralized systems such as robot

The goal of the robot swarm is to make a collective decision and to
reach consensus on the most frequent tile color of a black/white
grid. Each robot has a current opinion about the correct color, and
via dissemination/decision-making strategies, they influence their
peers. At the end of a successful run, all robots have the opinion of
the majority color (in our experiments it is always the white).

<p align="center">
<img src="https://github.com/Pold87/blockchain-swarm-robotics/blob/master/img/environment.png" alt="Collective decision-making scenario"/>
</p>



