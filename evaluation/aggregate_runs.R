source("myplothelpers.R")

## For paper:
## The good runs are 25-10-2017 and 28-10-2017 for the blockchain approach
## For the classical approach, the first runs included the buggy majority voting
## "24-10-2017" is also good but only contains 7, 8, and 9 Byzantine robots
num.byzantines = 0:9
difficulties <- c(34, 36, 38, 40, 42, 44, 46, 48)
##style <- "blockchain"
style <- "classical"
nodes <- 0:12
##data.base <- "/home/volker/localargdavide/data/"

do.experiment1 <- TRUE
do.experiment2 <- FALSE

dates.bc.exp1 <- c("08-11-2017", "04-11-2017", "03-11-2017")
dates.cl.exp1 <- c("30-10-2017")

dates.bc.exp2 <- c("04-11-2017", "03-11-2017", "31-10-2017", "28-10-2017", "25-10-2017", "24-10-2017")
dates.cl.exp2 <- c("30-10-2017")

if (style == "classical") {
    dates.exp1 <- dates.cl.exp1
    dates.exp2 <- dates.cl.exp2
} else {
    dates.exp1 <- dates.bc.exp1
    dates.exp2 <- dates.bc.exp2
}

data.base <- "~/local-exps/data/"
report.dir <- "~/Dropbox/mypapers/AAMAS2018/aamas18-latex-template/img/"
N = 20
strategies <- c(1, 2, 3)
do.consensus.on.correct.outcomes.only <- TRUE

dec2node <- function(dec) {
    if (dec == 1)
        return(0)
    if (dec == 2)
        return(2)
    if (dec == 3)
        return(5)
}

# Experiment 1 (Increasing difficulty)
create.df.exp1 <- function(max.trials=50) {
    df <- data.frame()
    for (s in strategies) {
        for (dat in dates.exp1) {
            for (i in 1:max.trials) {
                for (d in difficulties) {
                    for (node in nodes){
                        ##trials.name <- sprintf("%s/experiment1_decision%d-node%d-%s/num20_black%d_byz0_run%d.RUNS", data.base, s, dec2node(s), dat, d, i)

                        if (style == "classical") {                            
                            trials.name <- sprintf("%s/experiment1_decision%d-node%d-classical-%s/num20_black%d_byz0_run%d.RUNS", data.base, s, node, dat, d, i)
                        } else {
                            trials.name <- sprintf("%s/experiment1_decision%d-node%d-%s/num20_black%d_byz0_run%d.RUNS", data.base, s, node, dat, d, i)
                            }
                    if (file.exists(trials.name)) {
                        X <- read.table(trials.name, header=T)
                        if (nrow(X) != 0){ 
                            X$difficulty = d / (100 - d)
                            X$strat = s
                            if (nrow(df) == 0) {
                                df <- X
                            } else  {
                                df <- rbind(df, X)
                            }
                        }
                    }           
                    }
                }
            }
        }
    }
    return(df)    
}

# Experiment 2 (Byzantine robots)
create.df.exp2 <- function(max.trials=50) {
    df <- data.frame()
    for (s in strategies) {
        for (dat in dates.exp2) {
            for (i in 1:max.trials) {
                for (b in num.byzantines) {
                    if (style == "classical") {
                        trials.name <- sprintf("%s/experiment2_decision%d-node%d-byz-classical-%s/num20_black34_byz%d_run%d.RUNS", data.base, s, dec2node(s), dat, b, i)
                    } else {
                        trials.name <- sprintf("%s/experiment2_decision%d-node%d-byz-%s/num20_black34_byz%d_run%d.RUNS", data.base, s, dec2node(s), dat, b, i)
                        }
                    if (file.exists(trials.name)) {
                        X <- read.table(trials.name, header=T)
                        if (nrow(X) != 0){ 
                            X$num.byz = b
                            X$strat = s
                            if (nrow(df) == 0) {
                                df <- X
                            } else  {
                                df <- rbind(df, X)
                            }
                        }
                    }           
                }
            }
        }
    }
    return(df)    
}


data_summary <- function(data, varname, groupnames){
  require(plyr)
  summary_func <- function(x, col){
    c(mean = mean(x[[col]], na.rm=TRUE),
      sd = sd(x[[col]], na.rm=TRUE))
  }
  data_sum<-ddply(data, groupnames, .fun=summary_func,
                  varname)
  data_sum <- rename(data_sum, c("mean" = varname))
 return(data_sum)
}


if (do.experiment1){

    df <- create.df.exp1() ## Iterate over runs and create big df
    
    df$E.Ns <- df$Greens == N ## specify if the outcome was correct
    df.correct.only <- df[df$E.Ns == 1, ]

    df.correct.only$strat.names <- sapply(df.correct.only$strat, strat2strat.name) ## Insert strategy names

    
    df.agg <- aggregate(df, by=list(df$difficulty, df$strat), FUN=mean, na.rm=TRUE) ## aggregate via number of Byzantine robots
    df.agg2 <- data_summary(df, varname="ExitTime", groupnames = c("difficulty", "strat"))


    df.agg2$strat.names <- sapply(df.agg2$strat, strat2strat.name) ## Insert strategy names
    
    df.agg$strat.names <- sapply(df.agg$strat, strat2strat.name) ## Insert strategy names
    df.agg.correct.only <- aggregate(df.correct.only, by=list(df.correct.only$difficulty, df.correct.only$strat), FUN=mean, na.rm=TRUE) ## aggregate via number of Byzantine robots
    df.agg.correct.only$strat.names <- sapply(df.agg.correct.only$strat, strat2strat.name) ## Insert strategy names

    df$strat.names <- sapply(df$strat, strat2strat.name) ## Insert strategy names
    

 source("myplothelpers.R")
plot.exit.prob.gg.facet(df.agg,
                  xlab=expression("Difficulty ("* rho['b']^'*'*")"),
                  ylab=expression("Exit probability (E"[N]*")"),
                  sprintf("facet_exp1_exitprob_%s_correctonly%d.pdf", style, do.consensus.on.correct.outcomes.only),
                  report.dir)  


    source("myplothelpers.R")
    df.important <- df[, c("ExitTime", "difficulty", "strat.names")]
    plot.consensus.time.gg.box(df.correct.only,
                           xlab=expression("Difficulty ("* rho['b']^'*'*")"),                           
                           ylab=expression('Consensus time (T'['N']^'correct'*' / 10)'),
                           sprintf("box_exp1_consensustime_%s_correctonly%d.pdf", style, 1),
                           report.dir)
       
}

if (do.experiment2){

d <- 34
    
df <- create.df.exp2() ## Iterate over runs and create big df

    ## Remove crazy outliers
df <- df[df$ExitTime < 4000, ]
    
    
df$E.Ns <- df$Greens == (N - df$num.byz) ## specify if the outcome was correct
print(df[order(df$num.byz),]) 

## Remove all row with E.Ns != 1
df.correct.only <- df[df$E.Ns == 1, ]

## All runs
df.agg <- aggregate(df, by=list(df$num.byz, df$strat), FUN=mean, na.rm=TRUE) ## aggregate via number of Byzantine robots
df.agg$strat.names <- sapply(df.agg$strat, strat2strat.name) ## Insert strategy names

## Correct runs only
df.agg.correct.only <- aggregate(df.correct.only, by=list(df.correct.only$num.byz, df.correct.only$strat), FUN=mean, na.rm=TRUE) ## aggregate via number of Byzantine robots
df.agg.correct.only$strat.names <- sapply(df.agg.correct.only$strat, strat2strat.name) ## Insert strategy names


    df.correct.only$strat.names <- sapply(df.correct.only$strat, strat2strat.name) ## Insert strategy names
    
source("myplothelpers.R")
plot.exit.prob.gg.byz.facet(df.agg,
                      xlab="Number of Byzantine robots (k)",
                      ylab=expression("Exit probability (E"[N]*")"),
                      sprintf("facet_exp2_exitprob_%s_byz_diff%d_correctonly%d.pdf", style, d, do.consensus.on.correct.outcomes.only),
                      report.dir)  


source("myplothelpers.R")
plot.consensus.time.gg.byz.box(df.correct.only,
                      xlab="Number of Byzantine robots (k)",
                      ylab=expression('Sub-swarm consensus time (T'['N']^'correct'*' / 10)'),
                      sprintf("box_exp2_consensustime_%s_byz_diff%d_correctonly%d.pdf", style, d, 1),
                      report.dir)  

}
