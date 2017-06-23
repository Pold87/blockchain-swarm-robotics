source("myplothelpers.R")

## This file creates the plots for the experiments with the Byzantine robots

## '_combined' means that the runs are already aggregated in
## the .RUNS file (i.e., REPETITIONS is not used anymore)

## Settings
trials.base <- "volker"

use.fake.data <- FALSE
report.dir <- "~/Dropbox/mypapers/technical_report_collective/img/"
## Blockchain experiments
data.dir <- "../data/"

do.difficulty <- TRUE
do.consensus.on.correct.outcomes.only <- FALSE
max.trials <- 45
num.robots <- 20
ground.truth <- "Blacks"
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
#difficulty <- c(34, 36, 38, 40, 42, 44, 46, 48) # Make sure to match it with metastarter.sh

num.byzantines = 0:9



# As a function of Byzantine robots
if (do.difficulty) {
    
    E.Ns <- c()
    consensus.time <- c()
    num.byz <- c()
    strategies <- c(1, 2) ## TODO: Change back to c(1, 2, 3)
    k = num.robots
    
    strategy <- c()
    node <- 2
    runs <- c()
    d = 48

    
    for (s in strategies) {
            for (b in num.byzantines) {
               
                ## sprintf assumes that each strategy is executed on
                ## the corresponding node, e.g., strategy 2 -> node 2
                    f <- sprintf("%s/experiment1_decision%d-node%d-byzexp-next/num%d_black%d_byz%d_run0.RUNS", data.dir, s, s, k, d, b)    


                    if (file.exists(f)) {                

                        print(f)
                        
                        X <- read.table(f, header=T)

                        print(X)

                        successes <- X[, ground.truth] == (num.robots - b)

                        #print(successes)
                        #print(mean(successes))

                        # Find the subset of X where the robots agreed
                        # on the correct outcome

                        X.correct.outcome <- X[X[, ground.truth] == (num.robots - b), ]

                        print(X.correct.outcome)

                        print(mean(X.correct.outcome[, "ExitTime"]))
                        
                        #print(mean(X[, "ExitTime"]))

                        if (do.consensus.on.correct.outcomes.only) {

                            if (nrow(X.correct.outcome) == 0) {
                                m <- 0                                
                            } else {
                                m <-  mean(X.correct.outcome[, "ExitTime"])
                            }
                            consensus.time <- c(consensus.time, m)
                        } else {
                            if (nrow(X) == 0) {
                                m <- 0                                
                            } else {
                                m <-  mean(X[, "ExitTime"])
                            }
                            consensus.time <- c(consensus.time, m)
                        }
                        
                        E.Ns <- c(E.Ns, mean(successes))

                        strategy <- c(strategy, s)
                        num.byz <- c(num.byz, b)
                        
                        ## Number of runs
                        runs <- c(runs, length(successes))                    

                        
                        
            }
        }
    }

    df <- data.frame(num.byz, strategy, E.Ns, consensus.time, runs)

    plot.consensus.time.gg.byz(df,
                           xlab=expression("Number of Byzantine robots"),
                           ylab="Consensus time / 10",
                           sprintf("consensustime_blockchain_byz_diff%d.pdf", d))

    plot.exit.prob.gg.byz(df,
                      xlab="Number of Byzantine robots",
                      ylab="Exit probability",
                      sprintf("exitprob_blockchain_byz_diff%d.pdf", d))    

    
    
    
}
