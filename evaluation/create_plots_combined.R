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
                
                f <- sprintf("%s/experiment1_decision%s-node%s-classical-afterbugfix/num%d_black%d_byz0_run0.RUNS", data.dir, s, s, k, d)

                
                if (file.exists(f)) { 
                    print(f)
                    X <- read.table(f, header=T)
                    print(X)
                    
                    strategy <- c(strategy, s)
                    used.difficulties <- c(used.difficulties, d)
                    
                    ## Exit probability
                    successes <- X[, ground.truth] == k
                    E.Ns <- c(E.Ns, mean(successes))
                    
                    ## Consensus time
                    m <-  mean(X[, "ExitTime"])
                    print(m)
                    consensus.time <- c(consensus.time, m)
                    
                    ## Number of runs
                    runs <- c(runs, length(successes))
                    
                    
                }
            }
        }
    }

        
    ## Only plot data which is not NA
    #difficulty <- difficulty[!(is.na(E.Ns))]
    #difficulty <- difficulty / (100 - difficulty)
    #strategy <- strategy[!(is.na(E.Ns))]
    #E.Ns <- E.Ns[!(is.na(E.Ns))]

    #used.difficulties <- used.difficulties / (100 - used.difficulties)
    df <- data.frame(used.difficulties, E.Ns, strategy, consensus.time, runs)

    names(df) <- c("difficulty", "E.Ns", "strategy", "consensus.time", "runs")
    
    ## Save as PDF
    plot.consensus.time.gg(df,
                           xlab="Difficulty",
                           ylab="Consensus time / 10",
                           sprintf("consensustime_%d.pdf", k))
    plot.exit.prob.gg(df,
                      xlab="Difficulty", ylab="Exit probability",
                      sprintf("exit_prob_d_%d_gg.pdf", k))
        
}
