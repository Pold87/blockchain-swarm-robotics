source("myplothelpers.R")

## Settings
trials.base <- "volker"

use.fake.data <- FALSE
report.dir <- "~/Dropbox/mypapers/AAMAS2018/aamas18-latex-template/img_final_paper/"
#data.dir <- "../data/experiment1_decision3-run3/"
## Blockchain experiments
data.dir <- "../data/"
## Classical approach
#data.dir <- "../code_for_comparison/data/"

fake.data.dir <- "~/Documents/bc_collective/evaluation/"
#setwd(data.dir)

dates <- c("12-07-2017", "14-07-2017", "17-07-2017", "20-07-2017", "21-07-2017", "22-07-2017")

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
classical <- TRUE

dec2node <- function(dec) {
    if (dec == 1)
        return(1)
    if (dec == 2)
        return(3)
    if (dec == 3)
        return(5)
}


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


            X <- data.frame(Runs=integer(),
                            ExitTime=integer(),
                            Whites=integer(),
                            Greens=integer(),
                            Blacks=integer())
                
                
                for (date in dates) {

                    if (classical){
                        f <- sprintf("%s/experiment1_decision%s-node%s-classical-afterbugfix/num%d_black%d_byz0_run0.RUNS", data.dir, s, s, k, d)                       
                    } else {
                        f <- sprintf("%s/experiment1_decision%s-node%s-fast-redodiffs-%s/num%d_black%d_byz0_run0.RUNS", data.dir, s, dec2node(s), date, k, d)                        
                    }
                        
                    
                    if (file.exists(f)) { 
                        X.run <- read.table(f, header=T)
                        X <- rbind(X, X.run)
                    }
                }
                    
                    strategy <- c(strategy, s)
                    used.difficulties <- c(used.difficulties, d)
                    
                    ## Exit probability
                    successes <- X[, ground.truth] == k
                    E.Ns <- c(E.Ns, mean(successes))

                X.correct.outcome <- X[X[, ground.truth] == num.robots, ]
                
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
                                                
                ## Number of runs
                runs <- c(runs, length(successes))                    
                
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
        print(df)
    names(df) <- c("difficulty", "E.Ns", "strategy", "consensus.time", "runs")
    df$strat.names <- sapply(df$strategy, strat2strat.name)

        
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
                
        
    ## Save as PDF
    plot.consensus.time.gg(df,
                           xlab="Difficulty",
                           ylab=ylab.cons,
                           sprintf("consensustime_%s-_%d-redodiffs_correctonly%d.pdf", style, k, do.consensus.on.correct.outcomes.only),
                           report.dir)
    plot.exit.prob.gg(df,
                      xlab="Difficulty",
                      ylab=expression("Exit probability (E"[N]*")"),
                      sprintf("exit_prob_d_%s_%d_gg-redodiffs_correctonly%d.pdf", style, k, do.consensus.on.correct.outcomes.only),
                      report.dir)
        
    }
}
