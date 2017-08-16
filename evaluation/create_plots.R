source("myplothelpers.R")

## Settings
trials.base <- "volker"

use.fake.data <- FALSE
report.dir <- "~/Dropbox/mypapers/RAR/img/"
#data.dir <- "../data/experiment1_decision3-run3/"
## Blockchain experiments
data.dir <- "../data/"
## Classical approach
#data.dir <- "../code_for_comparison/data/"

fake.data.dir <- "~/Documents/bc_collective/evaluation/"
#setwd(data.dir)

max.trials <- 45
num.robots <- 20
ground.truth <- "Blacks"
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
difficulty <- c(34, 36, 38, 40, 42, 44, 46, 48) # Make sure to match it with metastarter.sh

## Determines which measures should be evaulated, e.g., if an
## experiment was run for determining the measures (consensus time,
## exit probability) as a function of the number of robots in the
## swarm
do.difficulty <- TRUE
do.number.of.robots <- FALSE
do.initial.amount <- FALSE


########################
### EXIT PROBABILITY ###
########################

# As a function of the difficulty of the task, easy and difficult setup
if (do.difficulty) {
    for (do.consensus.on.correct.outcomes.only in c(TRUE, FALSE)) {

        E.Ns <- c()
        strategies <- c(1, 2, 3)
        strategy <- c()
        nodes <- 0:20
        runs <- c()
        consensus.time <- c()
        
        ## Exit probability: As a function of difficulties
        strategy <- c()
        used.difficulties <- c()
        
        for (s in strategies) {
            for (k in num.robots) {    
                for (d in difficulty) {
                
                    used.difficulties <- c(used.difficulties, d)
                    successes <- c()                                        
                    strategy <- c(strategy, s)
                    consensus.times <- c()
                    
                    for (node in nodes) {
        ## For metastarter.sh
        ##trials.name <- sprintf("%s/exp_4_N%d_Percent%d", data.dir, k, d)

        ## For start_from_template.sh
        ## TODO: should include the name of the strategy in the filename
                        trials.name <- sprintf("%s/experiment1_decision%s-node%s/num%d_black%d", data.dir, s, node, k, d)

 ##                       trials.name <- sprintf("%s/experiment1_decision%s-node%s-classical-afterbugfix/num%d_black%d_byz0_run", data.dir, s, node, k, d)

                        
                    
        ## For all trials
        for (i in 0:max.trials) {
            f <- paste0(trials.name, i, ".RUNS")
            print(f)
            if (file.exists(f)) {
                X <- read.table(f, header=T)

                print(paste("strat is", s))
                
                if (!is.na(X[1, ground.truth])){ # Check that the run was completed

                    if (do.consensus.on.correct.outcomes.only) {

                        if (X[1, ground.truth] == k) {
                            consensus.times <- c(consensus.times, X[1, "ExitTime"])
                        } 
                    } else {
                        consensus.times <- c(consensus.times, X[1, "ExitTime"])
                    }
                    
                    if (X[1, ground.truth] == num.robots) {
                        successes <- c(successes, 1)
                    } else {
                        successes <- c(successes, 0)
                    }
                }
            }
        }
                    }
                    ## E.N is the exit probability
                                        #print(paste("The consensus time is", median(consensus.times)))
                    if (length(consensus.times) != 0) {
                        consensus.time <- c(consensus.time, median(consensus.times))
                    } else {
                        consensus.time <- c(consensus.time, 0)
                    }
                    
                    #print(paste("num.trials is", length(successes)))                
                    #print(paste("The exit probability is", mean(successes)))
                    E.Ns <- c(E.Ns, mean(successes))
                    runs <- c(runs, length(successes))

            }
        }
    }

    ## Only plot data which is not NA
    #difficulty <- difficulty[!(is.na(E.Ns))]
    #difficulty <- difficulty / (100 - difficulty)
    #strategy <- strategy[!(is.na(E.Ns))]
    #E.Ns <- E.Ns[!(is.na(E.Ns))]

    ##used.difficulties <- used.difficulties / (100 - used.difficulties)
    df <- data.frame(used.difficulties, E.Ns, strategy, consensus.time, runs)
            
    names(df) <- c("difficulty", "E.Ns", "strategy", "consensus.time", "runs")
    df$strat.names <- sapply(df$strategy, strat2strat.name)
    print(df)
    
    ## Save as PDF
    ##plot.exit.prob(df$difficulty, df$E.Ns,
    ##               xlab="Percentage white cells", ylab="Exit probability",
    ##               sprintf("exit_prob_d_%d.pdf", k))

    plot.consensus.time.gg(df,
                           xlab=expression("Difficulty"), ylab="Consensus time / 10",
                           sprintf("consensustime_blockchain_difficulty_correctonly%d.pdf", do.consensus.on.correct.outcomes.only))
    
    plot.exit.prob.gg(df,
                      xlab="Difficulty", ylab="Exit probability",
                      sprintf("exit_prob_blockchain_difficulty_correctonly%d.pdf", do.consensus.on.correct.outcomes.only))    
    }
}    

## if (do.number.of.robots) {
## ## Exit probability: As a function of the number of robots
## if (!use.fake.data) {
## for (i in num.robots) {
    
##     trials.name <- sprintf("%s_N_", trials.base, i)    
##     successes <- c()
    
##     ## For all trials
##     for (i in 1:max.trials) {
##         f <- paste0(trials.name, i, ".RUNS")
##         if (file.exists(f)) {
##             X <- read.table(f, header=T)
##             if (X[1, ground.truth] == num.robots) {
##                 successes <- c(successes, 1)
##             } else {
##                 successes <- c(successes, 0)
##                 }
##         }
##     }
##     ## E.N is the exit probability
##     print(paste("num.trials is", length(successes)))
##     print(paste("The exit probability is", mean(successes)))
##     exit.probs <- c(exit.probs, mean(successes))
## }
## df <- data.frame(num.robots, exit.probs)
## } else {
##     ## Import fake data
##     df <- read.csv(paste0(fake.data.dir, "fake_N.csv"))
## }

## ## Save as PDF
## plot.exit.prob(df$num.robots, df$exit.probs,
##                xlab="Number of robots in the swarm", ylab="Exit probability",
##                "exit_prob_N.pdf")


## ## Exit probability: As a function of the initial amount of robot favoring opinion black
## if (!use.fake.data) {
## for (i in num.robots) {
    
##     trials.name <- sprintf("%s_init_", trials.base, i)    
##     successes <- c()
    
##     ## For all trials
##     for (i in 1:max.trials) {
##         f <- paste0(trials.name, i, ".RUNS")
##         if (file.exists(f)) {
##             X <- read.table(f, header=T)
##             if (X[1, ground.truth] == num.robots) {
##                 successes <- c(successes, 1)
##             } else {
##                 successes <- c(successes, 0)
##                 }
##         }
##     }
##     ## E.N is the exit probability
##     print(paste("num.trials is", length(successes)))
##     print(paste("The exit probability is", mean(successes)))
##     exit.probs <- c(exit.probs, mean(successes))
## }
## df <- data.frame(num.robots, exit.probs)
## } else {
##     ## Import fake data
##     df <- read.csv(paste0(fake.data.dir, "fake_init.csv"))
## }

## ## Save in PDF
## plot.exit.prob(df$num.robots, df$exit.probs,
##                xlab="Initial number of robots favoring black", ylab="Exit probability",
##                "exit_prob_init.pdf")
##}
    
######################
### CONSENSUS TIME ###
######################

strategies <- c(1,2,3)
strategy <- c()
k <- num.robots

## Consensus time: As a function of the difficulty of the task
if (FALSE) {

    consensus.time <- c()
    strategy <- c()
    bc.height <- c()
    runs <- c()
    for (s in strategies) {
        for (d in difficulty) {

                used.difficulties <- c(used.difficulties, d)
                successes <- c()
                strategy <- c(strategy, s)
                consensus.times <- c()
                bc.heights <- c()
                
                for (node in nodes) {

                    trials.name <- sprintf("%s/experiment1_decision%s-node%s/num%d_black%d", data.dir, s, node, k, d)
                
                ## For all trials
                for (i in 1:max.trials) {
                    f <- paste0(trials.name, i, ".RUNS")
                    f.bc <- paste0(trials.name, i, "-blockchain.RUN1")

                    # Consensus time
                    if (file.exists(f)) {
                        X <- read.table(f, header=T)
                        if (!is.na(X[1, "ExitTime"])){ # Check that the run was completed

                            # Just for finding out why voter model is different with medium difficulty
#                            if (d == 42) {
#                                print(paste("strat is", s, "d is", d, "node is", node, "consensus time is", X[1, "ExitTime"]))
#                            }
                            
                            consensus.times <- c(consensus.times, X[1, "ExitTime"])

                            ## Blockchain height
                            if (file.exists(f.bc)) {
                                X.bc <- read.table(f.bc, header=T)
                                avg.height <- mean(unlist(X.bc[nrow(X.bc),2:ncol(X.bc)]))
                                bc.heights <- c(bc.heights, avg.height)
                            }                            
                        }
                    }
                }
                }

            print(paste("The consensus time is", median(consensus.times)))
            consensus.time <- c(consensus.time, median(consensus.times))
            runs <- c(runs, length(consensus.times))
            bc.height <- c(bc.height, mean(bc.heights))
    
        }
    }

#print(difficulty)
#print(bc.height)
#print(consensus.time)
#print(strategy)
#difficulty <- difficulty / (100 - difficulty)
#df <- data.frame(difficulty, consensus.time, strategy, runs)
#df.bc <- data.frame(difficulty, bc.height, strategy, runs)

## Save in PDF
#plot.consensus.time(df$difficulty, df$consensus.time,
#       xlab="Percentage white cells", ylab="Consensus time",
#       "consensustime_d.pdf")


#plot.consensus.time.gg(df,
#                       xlab=expression("Difficulty"), ylab="Consensus time / 10",
#                       "consensustime_d_gg.pdf")
    
#plot.bc.height.gg(df.bc,
#                       xlab=expression("Difficulty"), ylab="Blockchain height",
#                       "blockchainheight_gg.pdf")


}



## Consensus time: As a function of the number of robots in the swarm
if (do.number.of.robots) {
if (!use.fake.data) {

    consensus.time <- c()
    
for (i in num.robots) {

    ## For metastarter.sh
    trials.name <- sprintf("%s_init_", trials.base, i)
    
    consensus.times <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            consensus.times <- c(consensus.times, X[1, "ExitTime"])
        }
    }
    print(paste("The consensus time is", mean(consensus.times)))
    consensus.time <- c(consensus.time, mean(consensus.times))
}
df <- data.frame(num.robots, consensus.time)
} else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_N_consensustime.csv"))
}

## Save in PDF
plot.consensus.time(df$num.robots, df$consensus.time,
       xlab="Number of robots", ylab="Consensus time",
       "consensustime_N.pdf")

}


## Consensus time: As a function of the initial amount of robot favoring opinion black
if (do.initial.amount) {
if (!use.fake.data) {

    consensus.time <- c()
    
    for (i in num.robots) {
    
    trials.name <- sprintf("%s_init_", trials.base, i)    
    consensus.times <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            consensus.times <- c(consensus.times, X[1, "ExitTime"])
        }
    }
    print(paste("The consensus time is", median(consensus.times)))
    consensus.time <- c(consensus.time, median(consensus.times))
}
df <- data.frame(num.robots, consensus.time)
} else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_init_consensustime.csv"))
}

## Save in PDF
plot.consensus.time(df$num.robots, df$consensus.time,
       xlab="Initial Number of robots favoring black", ylab="Consensus time",
       "consensustime_init.pdf")

}
