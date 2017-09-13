source("myplothelpers.R")

## This file creates the plots for the experiments with the Byzantine robots

## '_combined' means that the runs are already aggregated in
## the .RUNS file (i.e., REPETITIONS is not used anymore)

## Settings

dates <- c("30-06-2017", "03-07-2017", "05-07-2017",
           "07-07-2017", "08-07-2017", "08-07-2017",
           "08-07-2017", "22-07-2017", "25-07-2017")
trials.base <- "volker"

use.fake.data <- FALSE
report.dir <- "~/Dropbox/mypapers/RAR/img/"
## Blockchain experiments
data.dir <- "../data/"

do.difficulty <- TRUE
max.trials <- 45
num.robots <- 20
ground.truth <- "Blacks"
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
#difficulty <- c(34, 36, 38, 40, 42, 44, 46, 48) # Make sure to match it with metastarter.sh

num.byzantines = 1:9
diffs <- c(34, 48)
classical <- FALSE

dec2node <- function(dec) {
    if (dec == 1)
        return(0)
    if (dec == 2)
        return(3)
    if (dec == 3)
        return(1)
}


## As a function of Byzantine robots
if (do.difficulty) {
    for (d in diffs) {
        for (do.consensus.on.correct.outcomes.only in c(TRUE, FALSE)) {
    
    E.Ns <- c()
    consensus.time <- c()
    num.byz <- c()
    strategies <- c(1, 2, 3) ## TODO: Change back to c(1, 2, 3)
    k = num.robots
    
    strategy <- c()
    runs <- c()
    print("Difficulty is ", str(d))

    for (s in strategies) {
        for (b in num.byzantines) {

            existed <- FALSE

            X <- data.frame(Runs=integer(),
                            ExitTime=integer(),
                            Whites=integer(),
                            Greens=integer(),
                            Blacks=integer())

            if (classical) {
                f <- sprintf("%s/experiment1_decision%s-node%s-classical/num%d_black%d_byz%d_run0.RUNS", data.dir, s, s, k, d, b)

                if (file.exists(f)) {
                    X <- read.table(f, header=T)
                    print(X)
                    
                    }
                
                } else {
                    for (date in dates) {
                        
                        ## sprintf assumes that each strategy is executed on
                        ## the corresponding node, e.g., strategy 2 -> node 2
                        f <- sprintf("%s/experiment1_decision%d-node%d-byzexp-fastcorrect-%s/num%d_black%d_byz%d_run0.RUNS", data.dir, s, dec2node(s), date, k, d, b)    
                        ##f <- sprintf("%s/experiment1_decision%d-node%d-classical/num%d_black%d_byz%d_run0.RUNS", data.dir, s, s, k, d, b)    
                        
                        if (file.exists(f)) {                
                            
                            X.run <- read.table(f, header=T)
                            
                            X <- rbind(X, X.run)
                            
                            ##print(f)
                            
                        } else if (file.exists(sprintf("%s/experiment1_decision%d-node%d-fast-byzantine-again-%s/num%d_black%d_byz%d_run0.RUNS", data.dir, s, dec2node(s), date, k, d, b))) {

                            f <- sprintf("%s/experiment1_decision%d-node%d-fast-byzantine-again-%s/num%d_black%d_byz%d_run0.RUNS", data.dir, s, dec2node(s), date, k, d, b)
                            
                            X.run <- read.table(f, header=T)
                            X <- rbind(X, X.run)


                        }
                    }
                }
            
            existed <- TRUE
                        


            successes <- X[, ground.truth] == (num.robots - b)

            ##print(successes)
            ##print(mean(successes))

            ## Find the subset of X where the robots agreed
            ## on the correct outcome

            X.correct.outcome <- X[X[, ground.truth] == (num.robots - b), ]

            ##print(X.correct.outcome)

            ##print(mean(X.correct.outcome[, "ExitTime"]))
            
            ##print(mean(X[, "ExitTime"]))

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

                        if (length(successes) == 0) {
                            E.Ns <- c(E.Ns, 0)
                            
                        } else {
                            E.Ns <- c(E.Ns, mean(successes))
                        }

                        strategy <- c(strategy, s)
                        num.byz <- c(num.byz, b)
                        
                        ## Number of runs
                        runs <- c(runs, length(successes))                    

                        
                        
                    }

                if (!existed) {

                    consensus.time <- c(consensus.time, 0)
                    E.Ns <- c(E.Ns, 0)                          
                    
                    strategy <- c(strategy, s)
                    num.byz <- c(num.byz, b)
                        
                    runs <- c(runs, 0)                    
                }
        }

            df <- data.frame(num.byz, strategy, E.Ns, consensus.time, runs)
            df$strat.names <- sapply(df$strategy, strat2strat.name)
    #print(df)

            if (classical) {
                style <- "classical"
            } else {
                style <- "blockchain"
            }


            if (do.consensus.on.correct.outcomes.only) {
                ylab.cons <- expression('Consensus time (T'['N']^'correct'*' / 10)')
            } else {
                ylab.cons <- expression('Consensus time (T'['N']^'all'*' / 10)')
            }
            
            
    plot.consensus.time.gg.byz(df,
                           xlab=expression("Number of Byzantine robots"),
                           ylab=ylab.cons,
                           sprintf("consensustime_%s_byz_diff%d_correctonly%d.pdf", style, d, do.consensus.on.correct.outcomes.only))

    plot.exit.prob.gg.byz(df,
                      xlab="Number of Byzantine robots",
                      ylab=expression("Exit probability (E"[N]*")"),
                      sprintf("exitprob_%s_byz_diff%d_correctonly%d.pdf", style, d, do.consensus.on.correct.outcomes.only))    
    
    }
}
}








