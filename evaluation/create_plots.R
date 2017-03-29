library(Hmisc)
library(plotrix)

## Settings
trials.base <- "volker"

use.fake.data <- FALSE

report.dir <- "/home/volker/Dropbox/mypapers/technical_report_collective/img/"
data.dir <- "../data/first_good_run/"
fake.data.dir <- "/home/volker/Documents/bc_collective/evaluation/"
#setwd(data.dir)

max.trials <- 45
num.robots <- 10
ground.truth <- "Blacks"
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
#difficulty <- c(52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96) # Make sure to match it with metastarter.sh
difficulty <- c(0) # Make sure to match it with metastarter.sh

## Plot function

## Plot exit probability 
plot.exit.prob <- function(x, y, xlab, ylab, out.name) {

pdf(paste0(report.dir, out.name))
par(cex.lab = 1.7, las=1, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(x, y,
     xlab=xlab, ylab=ylab,
     pch=3, col=tol4qualitative[1], type="b", ylim=c(0, 1), font.lab=2, cex=2,
     lty=2)
abline(h =1, untf = FALSE, lty=2)
abline(h =0, untf = FALSE, lty=2)
legend(x="bottomright", legend=c("BC strategy"),
       col=tol4qualitative[1:1],
       pch=2:2,
       lty=2:2,
       cex=2,
       bty="n")
dev.off() 
}

## Plot conensus time
plot.consensus.time <- function(x, y, xlab, ylab, out.name) {

pdf(paste0(report.dir, out.name))
par(cex.lab = 1.7, las=1,cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(x, y,
     xlab=xlab, ylab=ylab,
     pch=3, col=tol4qualitative[1], type="b", ylim=c(0, max(y)), font.lab=2, cex=2,
     lty=2)
legend(x="bottomright", legend=c("BC strategy"),
       col=tol4qualitative[1:1],
       pch=2:2,
       lty=2:2,
       cex=2,
       bty="n")
dev.off() 
}


########################
### EXIT PROBABILITY ###
########################

# As a function of the difficulty of the task, easy and difficult setup

exit.probs <- c()
## Exit probability: As a function of difficulties
if (!use.fake.data) {
for (k in num.robots) {    
    for (d in difficulty) {

        ## For metastarter.sh
        ##trials.name <- sprintf("%s/exp_4_N%d_Percent%d", data.dir, k, d)

        ## For start_from_template.sh
        trials.name <- sprintf("%s/num%d_black%d", data.dir, k, d)    
        successes <- c()
    
        ## For all trials
        for (i in 1:max.trials) {
            f <- paste0(trials.name, i, ".RUNS")
            if (file.exists(f)) {
                X <- read.table(f, header=T)
                if (X[1, ground.truth] == num.robots) {
                    successes <- c(successes, 1)
                } else {
                    successes <- c(successes, 0)
                }
            }
        }
        ## E.N is the exit probability
        print(paste("num.trials is", length(successes)))
        print(paste("The exit probability is", mean(successes)))
        exit.probs <- c(exit.probs, mean(successes))
    }
    df <- data.frame(difficulty, exit.probs)
    ## Save as PDF
    plot.exit.prob(df$difficulty, df$exit.probs,
                   xlab="Percentage white cells", ylab="Exit probability",
                   sprintf("exit_prob_d_%d.pdf", k))
    }
    } else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_d.csv"))
    ## Save as PDF
    plot.exit.prob(df$difficulty, df$exit.probs,
                   xlab="Percentage white cells", ylab="Exit probability",
                   "exit_prob_d_fake.pdf")
    }


## Exit probability: As a function of the number of robots
if (!use.fake.data) {
for (i in num.robots) {
    
    trials.name <- sprintf("%s_N_", trials.base, i)    
    successes <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            if (X[1, ground.truth] == num.robots) {
                successes <- c(successes, 1)
            } else {
                successes <- c(successes, 0)
                }
        }
    }
    ## E.N is the exit probability
    print(paste("num.trials is", length(successes)))
    print(paste("The exit probability is", mean(successes)))
    exit.probs <- c(exit.probs, mean(successes))
}
df <- data.frame(num.robots, exit.probs)
} else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_N.csv"))
}

## Save as PDF
plot.exit.prob(df$num.robots, df$exit.probs,
               xlab="Number of robots in the swarm", ylab="Exit probability",
               "exit_prob_N.pdf")


## Exit probability: As a function of the initial amount of robot favoring opinion black
if (!use.fake.data) {
for (i in num.robots) {
    
    trials.name <- sprintf("%s_init_", trials.base, i)    
    successes <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            if (X[1, ground.truth] == num.robots) {
                successes <- c(successes, 1)
            } else {
                successes <- c(successes, 0)
                }
        }
    }
    ## E.N is the exit probability
    print(paste("num.trials is", length(successes)))
    print(paste("The exit probability is", mean(successes)))
    exit.probs <- c(exit.probs, mean(successes))
}
df <- data.frame(num.robots, exit.probs)
} else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_init.csv"))
}

## Save in PDF
plot.exit.prob(df$num.robots, df$exit.probs,
               xlab="Initial number of robots favoring black", ylab="Exit probability",
               "exit_prob_init.pdf")

######################
### CONSENSUS TIME ###
######################


## Consensus time: As a function of the difficulty of the task
if (!use.fake.data) {
for (d in difficulty) {
    
    trials.name <- sprintf("%s_red%d_blue%d", trials.base, d, 100 - d)    
    successes <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            consensus.times <- c(consensus.times, X[1, clock]) 
        }
    }

    print(paste("The consensus time is", median(consensus.times)))
    consensus.time <- c(consensus.time, median(consensus.times))
    
}
df <- data.frame(difficulty, consensus.time)
} else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_d_consensustime.csv"))
}

## Save in PDF
plot.consensus.time(df$difficulty, df$consensus.time,
       xlab="Percentage white cells", ylab="Consensus time",
       "consensustime_d.pdf")


## Consensus time: As a function of the number of robots in the swarm
if (!use.fake.data) {
for (i in num.robots) {

    ## For metastarter.sh
    trials.name <- sprintf("%s_init_", trials.base, i)
    
    consensus.times <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            consensus.times <- c(consensus.times, X[1, clock])
        }
    }
    print(paste("The consensus time is", median(consensus.times)))
    consensus.time <- c(consensus.time, median(consensus.times))
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


## Consensus time: As a function of the initial amount of robot favoring opinion black
if (!use.fake.data) {
for (i in num.robots) {
    
    trials.name <- sprintf("%s_init_", trials.base, i)    
    consensus.times <- c()
    
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            X <- read.table(f, header=T)
            consensus.times <- c(consensus.times, X[1, clock])
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
