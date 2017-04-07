#library(Hmisc)
#library(plotrix)
library(ggplot2)
library(ggthemes)
library(directlabels)
library(grid)
## Settings
trials.base <- "volker"

use.fake.data <- TRUE


report.dir <- "/home/volker/Dropbox/mypapers/technical_report_collective/img/"
data.dir <- "../data/debug/"
fake.data.dir <- "/home/volker/Documents/bc_collective/evaluation/"

#setwd(data.dir)

max.trials <- 45
num.robots <- 10
ground.truth <- "Blacks"
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
difficulty <- c(0, 10, 20, 30, 40, 44, 48) # Make sure to match it with metastarter.sh

## Determines which measures should be evaulated, e.g., if an
## experiment was run for determining the measures (consensus time,
## exit probability) as a function of the number of robots in the
## swarm
do.difficulty <- TRUE
do.number.of.robots <- FALSE
do.initial.amount <- FALSE


base_breaks_x <- function(x){
  b <- x
  d <- data.frame(y=-Inf, yend=-Inf, x=min(b), xend=max(b))
  list(geom_segment(data=d, size=1, colour="gray35", aes(x=x, y=y, xend=xend, yend=yend), inherit.aes=FALSE),
#       geom_segment(data=d, size=1, colour="white", aes(x=xend, y = y, xend = xend+0.2, yend=yend), inherit.aes=FALSE),
       scale_x_continuous(breaks=b))
}
base_breaks_y <- function(x){
  b <- x
  d <- data.frame(x=-Inf, xend=-Inf, y=min(b), yend=max(b))
  list(geom_segment(data=d, size=1, colour="gray35", aes(x=x, y=y, xend=xend, yend=yend), inherit.aes=FALSE),
       scale_y_continuous(breaks=b))
}


## Plot function

## Plot exit probability 
plot.exit.prob <- function(x, y, xlab, ylab, out.name) {

pdf(paste0(report.dir, out.name))
par(cex.lab = 1.7, las=1, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
p <- plot(x, y,
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


plot.exit.prob.gg <- function(df, xlab, ylab, out.name) {

    df[, 'strategy'] <- as.factor(df[, 'strategy'])
    p <- ggplot(df, aes(x=difficulty, y=E.Ns, group=strategy)) +
        geom_line(aes(colour = strategy), size=1.1) +
        geom_point(aes(colour = strategy, shape = strategy), size=3) +
        theme_classic() +
        theme(axis.text=element_text(size=17, colour="gray25"),
              axis.title=element_text(size=17, colour="gray25"),
              axis.title.y = element_text(angle=0, margin = margin(r = -80), vjust=1.01),
              axis.line = element_blank(),              
              axis.ticks.length=unit(-0.25, "cm"),
              axis.ticks = element_line(colour = 'gray25'),
              axis.text.x = element_text(margin=unit(c(0.5,0.5,0.5,0.5), "cm")),
              axis.text.y = element_text(margin=unit(c(0.5,0.5,0.5,0.5), "cm")))  +
        ylab(ylab) +
        xlab(xlab) +

    base_breaks_x(seq(0.5, 1, 0.1)) + 
    base_breaks_y(seq(0.4, 1, 0.1)) + 
        
    expand_limits(x = 1.05)

    p <- direct.label(p, list(dl.trans(x=x+0.2, y=y),
                              list("last.qp", cex=1.4)))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
        
    #p <- direct.label(p, "last.qp")
    ggsave(paste0(report.dir, "ggplot_exit.pdf"))
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

## Plot conensus time
plot.consensus.time.gg <- function(df, xlab, ylab, out.name) {

    df[, 'strategy'] <- as.factor(df[, 'strategy'])
    p <- ggplot(df, aes(x=Ea0, y=consensus.time / 10, group=strategy)) +
        geom_line(aes(colour = strategy), size=1.1) +
        geom_point(aes(colour = strategy, shape = strategy), size=3) +
        theme_classic() +
        theme(axis.text=element_text(size=17, colour="gray15"),
              axis.title=element_text(size=17, colour="gray15"),
              axis.title.y = element_text(angle=0, margin = margin(r = -60, t = -60)),
              panel.border = element_blank(),
              panel.grid.major = element_blank(),
              panel.grid.minor = element_blank(),
              axis.line = element_blank(),
              axis.ticks.length=unit(-0.25, "cm"),
              axis.ticks = element_line(colour = 'gray15'),
              axis.text.x = element_text(margin=unit(c(0.5,0.5,0.5,0.5), "cm")),
              axis.text.y = element_text(margin=unit(c(0.5,0.5,0.5,0.5), "cm")))  +
        ylab(ylab) +
        xlab(xlab) + 
        base_breaks_x(seq(0, 20, 5)) +
        base_breaks_y(seq(0, 20, 5)) + expand_limits(x=25)


    p <- direct.label(p, list(dl.trans(x=x+0.2, y=y),
                              list("last.qp", cex=1.0)))
    
   # p <- direct.label(p, list(dl.trans(x=x-2.5, y=y+0.4), "last.qp"))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
    
    ggsave(paste0(report.dir, "ggplot_consensus.pdf"))
    }



########################
### EXIT PROBABILITY ###
########################

# As a function of the difficulty of the task, easy and difficult setup
if (do.difficulty) {

E.Ns <- c()
strategies <- c(2)
strategy <- c()
## Exit probability: As a function of difficulties
if (!use.fake.data) {

    strategy <- c()
    
    for (s in strategies) {
        for (k in num.robots) {    
            for (d in difficulty) {

        ## For metastarter.sh
        ##trials.name <- sprintf("%s/exp_4_N%d_Percent%d", data.dir, k, d)

        ## For start_from_template.sh
        ## TODO: should include the name of the strategy in the filename
        trials.name <- sprintf("%s/num%d_black%d", data.dir, k, d)    
        successes <- c()
        strategy <- c(strategy, s)
                
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
        E.Ns <- c(E.Ns, mean(successes))
            }
        }
    }
    df <- data.frame(difficulty, E.Ns, strategy)
    ## Save as PDF
    plot.exit.prob(df$difficulty, df$E.Ns,
                   xlab="Percentage white cells", ylab="Exit probability",
                   sprintf("exit_prob_d_%d.pdf", k))
    plot.exit.prob.gg(df,
                      xlab="Percentage white cells", ylab="Exit probability",
                      "exit_prob_d_fake.pdf")
    
    } else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_python.csv"))
    ## Save as PDF
    plot.exit.prob(df$difficulty, df$E.Ns,
                   xlab="Percentage white cells", ylab="Exit probability",
                   "exit_prob_d_fake.pdf")
    plot.exit.prob.gg(df,
                      xlab=expression(paste("Problem difficulty (", rho["w"]^'*', ")")), ylab=expression(paste("Exit probability (EN)")),
                      "exit_prob_d_fake.pdf")
    }

}
    

if (do.number.of.robots) {
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
}
    
######################
### CONSENSUS TIME ###
######################

strategies <- c(2)
strategy <- c()

## Consensus time: As a function of the difficulty of the task
if (do.difficulty) {
if (!use.fake.data) {

    consensus.time <- c()
    strategy <- c()
    for (s in strategies) {
        for (d in difficulty) {
            for (k in num.robots) {    
    
                trials.name <- sprintf("%s/num%d_black%d", data.dir, k, d)    
                successes <- c()
                consensus.times <- c()

                strategy <- c(strategy, s)
                
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
        }
    }

    print(difficulty)
    print(consensus.time)
    print(strategy)
df <- data.frame(difficulty, consensus.time, strategy)
} else {
    ## Import fake data
    df <- read.csv(paste0(fake.data.dir, "fake_python_cons.csv"))
}

## Save in PDF
#plot.consensus.time(df$difficulty, df$consensus.time,
#       xlab="Percentage white cells", ylab="Consensus time",
#       "consensustime_d.pdf")

plot.consensus.time.gg(df,
                       xlab=expression("Initial number of robots with opinion w: E[a](0)"), ylab="Consensus time (T[N] / 10)",
                       "consensustime_d_gg.pdf")


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
