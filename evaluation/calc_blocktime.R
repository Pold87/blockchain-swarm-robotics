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
E.BH <- c() ## Block height

for (d in difficulties) {
    trials.name <- sprintf("volker_red%d_blue%d", d, 100 - d)    
    ## For all difficulties
    longest.chains <- c()
    num.trials <- 0
    ## For all trials
    for (i in 1:max.trials) {
        f <- paste0(trials.name, i, "-blockchain.RUN1")
        if (file.exists(f)) {
            num.trials <- num.trials + 1
            X <- read.table(f, header=T)
            ## Find longest chain
            longest.chain <- max(X[-1, 2:ncol(X)])
            longest.chains <- c(longest.chains, longest.chain)
        }
    }    
    E.BH <- c(E.BH, median(longest.chains))
    print(paste("num.trials is", num.trials))
}

df <- data.frame(difficulties, E.BH)
pdf("/home/volker/Dropbox/mypapers/technical_report_collective/img/median_block_height.pdf")
par(cex.lab = 1.7, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(df$difficulties, df$E.BH,
     xlab="Percentage white cells", ylab="Median block height",
     pch=3, col=tol4qualitative[1], type="b", font.lab=2, cex=2,
     lty=2)

## Plot second strategy
legend(x="bottomright", legend=c("BC strategy"),
       col=tol4qualitative[1:1],
       pch=2:2,
       lty=2:2,
       cex=2,
       bty="n")
dev.off()
