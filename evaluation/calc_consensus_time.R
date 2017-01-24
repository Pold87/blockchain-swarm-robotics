## Settings
##trials.name <- "volker"
dir <- "new_data/"
num.trials <- 8
num.robots <- 10
difficulties <- c(52, 60, 65, 70, 80)
tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
## Apply settings
setwd(dir)
## The data frame that holds the data to plot
E.Ts <- c()
for (d in difficulties) {
    trials.name <- sprintf("volker_red%d_blue%d", d, 100 - d)    
    ## For all difficulties
    cs <- c()
    ## For all trials
    for (i in 1:num.trials) {
        f <- paste0(trials.name, i, ".RUNS")
        X <- read.table(f, header=T)
        print(X[1, "ExitTime"])
        cs <- c(cs, X[1, "ExitTime"])
    }
    ## E.T is the median exit time
    E.T <- median(cs)
    E.Ts <- c(E.Ts, E.T)    
    print(paste("The exit time is", E.T))    
}

df <- data.frame(difficulties, E.Ts)

pdf("/home/volker/Dropbox/mypapers/technical_report_collective/img/cons_time.pdf")
par(cex.lab = 1.7, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(df$difficulties, df$E.Ts,
     xlab="Percentage white cells", ylab="Consensus time",
     pch=3, col=tol4qualitative[1], type="b", font.lab=2, cex=2,
     lty=2)
abline(h =0, untf = FALSE, lty=2)
## Plot second strategy
legend(x="bottomright", legend=c("BC strategy"),
       col=tol4qualitative[1:1],
       pch=2:2,
       lty=2:2,
       cex=2,
       bty="n")
dev.off()
