## Settings
##trials.name <- "volker"
library(Hmisc)
library(plotrix)
dir <- "blockchain-run/"
max.trials <- 45
num.robots <- 10
ground.truth <- "Blues"
difficulties <- c(52, 56, 60, 64, 68, 72, 76)
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
## Apply settings
setwd(dir)
## The data frame that holds the data to plot
E.Ns <- c()
E.LI <- c()
E.UI <- c()
for (d in difficulties) {
    trials.name <- sprintf("volker_red%d_blue%d", d, 100 - d)    
    ## For all difficulties
    c <- 0
    num.trials <- 0
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        if (file.exists(f)) {
            num.trials <- num.trials + 1
            X <- read.table(f, header=T)
            if (X[1, ground.truth] == num.robots) {
                c <- c + 1
            }
        }
    }
    ## E.N is the exit probability
    print(paste("num.trials is", num.trials))
    b <- binconf(x = c, n = num.trials, alpha=0.05)
    #E.N <- c / num.trials
    E.Ns <- c(E.Ns, b[1])
    E.LI <- c(E.LI, b[2])
    E.UI <- c(E.UI, b[3])
    print(paste("The exit probability is", E.Ns))    
}
df <- data.frame(difficulties, E.Ns, E.LI, E.UI)
pdf("/home/volker/Dropbox/mypapers/technical_report_collective/img/exit_prob_N.pdf")
par(cex.lab = 1.7, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(df$difficulties, df$E.Ns,
     xlab="Number of robots in the swarm", ylab="Exit probability",
     pch=3, col=tol4qualitative[1], type="b", ylim=c(0, 1), font.lab=2, cex=2,
     lty=2)
abline(h =1, untf = FALSE, lty=2)
abline(h =0, untf = FALSE, lty=2)
## Plot second strategy
legend(x="bottomright", legend=c("BC strategy"),
       col=tol4qualitative[1:1],
       pch=2:2,
       lty=2:2,
       cex=2,
       bty="n")
dev.off()
