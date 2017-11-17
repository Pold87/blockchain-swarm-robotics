# Managing Byzantine Robots via Blockchain Technology in a Swarm Robotics Collective Decision Making Scenario


This repository contains the code for the paper "Managing Byzantine
Robots via Blockchain Technology in a Swarm Robotics Collective
Decision Making Scenario."

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

![Collective decision-making scenario](https://raw.githubusercontent.com/Pold87/blockchain-swarm-robotics/master/img/ "Collective decision-making scenario")



