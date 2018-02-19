# Managing Byzantine Robots via Blockchain Technology in a Swarm Robotics Collective Decision Making Scenario


This repository contains the code for the paper:

V.Strobel, E. Castello Ferrer, M. Dorigo. 2018.
Managing Byzantine Robots via Blockchain Technology in a Swarm Robotics Collective Decision Making Scenario.

*To appear in Proceedings of the 17th Conference on Autonomous Agents and MultiAgent Systems. International Foundation for Autonomous Agents and Multiagent Systems (AAMAS 2018).*

A pre-print is available at:  
http://iridia.ulb.ac.be/IridiaTrSeries/


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



