source("myplothelpers.R")

## Settings
trials.base <- "volker"

use.fake.data <- FALSE
report.dir <- "~/Dropbox/mypapers/technical_report_collective/img/"
#data.dir <- "../data/experiment1_decision3-run3/"
## Blockchain experiments
data.dir <- "../data/"
## Classical approach
#data.dir <- "../code_for_comparison/data/"

fake.data.dir <- "~/Documents/bc_collective/evaluation/"
#setwd(data.dir)

do.difficulty <- TRUE
max.trials <- 45
num.robots <- 20
ground.truth <- "Blacks"
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
#difficulty <- c(34, 36, 38, 40, 42, 44, 46, 48) # Make sure to match it with metastarter.sh

num.byzantines = 0:9
diffs <- c(34, 48)

# As a function of Byzantine robots, easy and difficult setup
if (do.difficulty) {
    for (d in diffs) {
    E.Ns <- c()
    strategies <- c(0)
    strategy <- c()
    nodes <- 0:2
    runs <- c()
    
    ## Exit probability: As a function of Byzantine robots
    
    strategy <- c()
    used.difficulties <- c()
    
    for (s in strategies) {
            for (k in num.robots) {    
                for (b in num.byzantines) {

                    successes <- c()                                        
                    strategy <- c(strategy, s)
                    
                    for (node in nodes) {
                        trials.name <- sprintf("%s/experiment1_decision%s-node%s-classical-afterbugfix/num%d_black%d_byz%d_run", data.dir, s, node, k, d, b)    
                    
        ## For all trials
        for (i in 1:max.trials) {
            f <- paste0(trials.name, i, ".RUNS")
            if (file.exists(f)) {
                X <- read.table(f, header=T)
                if (!is.na(X[1, ground.truth])){ # Check that the run was completed
                    if (X[1, ground.truth] == (num.robots - b)) {
                        successes <- c(successes, 1)
                    } else {
                        successes <- c(successes, 0)
                    }
                }
            }
        }
                    }
        ## E.N is the exit probability
        print(paste("num.trials is", length(successes)))                
        print(paste("The exit probability is", mean(successes)))
        E.Ns <- c(E.Ns, mean(successes))
        runs <- c(runs, length(successes))
            }
        }
    }

    #used.difficulties <- used.difficulties / (100 - used.difficulties)
    df <- data.frame(num.byzantines, E.Ns, strategy, runs)

    names(df) <- c("difficulty", "E.Ns", "strategy", "runs")
    
    ## Save as PDF
    plot.exit.prob.gg(df,
                      xlab="Number of Byzantine robots", ylab="Exit probability",
                      sprintf("exit_prob_d_%d_gg_diff%d.pdf", k, d))
    
}
    

strategies <- c(0)
strategy <- c()
k <- num.robots

## Consensus time: As a function of the difficulty of the task
if (do.difficulty) {

    d = 34
    
    consensus.time <- c()
    strategy <- c()
    bc.height <- c()

    runs <- c()
    
    for (s in strategies) {
        for (b in num.byzantines) {

                used.difficulties <- c(used.difficulties, d)
                successes <- c()
                strategy <- c(strategy, s)
                consensus.times <- c()
                bc.heights <- c()
                
                for (node in nodes) {

                    trials.name <- sprintf("%s/experiment1_decision%s-node%s-classical/num%d_black%d_byz%d_run", data.dir, s, node, k, d, b)    
                
                ## For all trials
                for (i in 1:max.trials) {
                    f <- paste0(trials.name, i, ".RUNS")
                    f.bc <- paste0(trials.name, i, "-blockchain.RUN1")

                    print(f)
                    
                    # Consensus time
                    if (file.exists(f)) {


                        
                        X <- read.table(f, header=T)
                        if (!is.na(X[1, "ExitTime"])){ # Check that the run was completed

                            # Just for finding out why voter model is different with medium difficulty
                            if (d == 42) {
                                print(paste("strat is", s, "d is", d, "node is", node, "consensus time is", X[1, "ExitTime"]))
                            }
                            
                            consensus.times <- c(consensus.times, X[1, "ExitTime"])

                            ## Blockchain height
                            if (file.exists(f.bc)) {
                                X.bc <- read.table(f.bc, header=T)
                                avg.height <- mean(unlist(X.bc[nrow(X.bc),2:ncol(X.bc)]))
                                ## bc.heights <- c(bc.heights, avg.height)
                            }                            
                        }
                    }
                }
                }
            print(paste("The consensus time is", mean(consensus.times)))
            consensus.time <- c(consensus.time, mean(consensus.times))
            ## bc.height <- c(bc.height, mean(bc.heights))
#            runs <- c(runs, length(successes))
    
        }
    }

#print(difficulty)
#print(bc.height)
#print(consensus.time)
#print(strategy)
#difficulty <- difficulty / (100 - difficulty)
    df <- data.frame(num.byzantines, consensus.time, strategy)
    print(df)
    names(df) <- c("difficulty", "consensus.time", "strategy")
plot.consensus.time.gg(df,
                       xlab=expression("Difficulty"), ylab="Consensus time / 10",
                       sprintf("consensustime_d_gg_byzantine_diff%d.pdf", d))
}
