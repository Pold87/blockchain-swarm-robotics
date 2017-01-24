## Settings
##trials.name <- "volker"
dir <- "fullyconnected/"
file.name <- "volker_num10_red64_blue361-blockchain.RUN1"
#tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
tol12qualitative=c("#332288", "#6699CC", "#88CCEE", "#44AA99", "#117733", "#999933", "#DDCC77", "#661100", "#CC6677", "#AA4466", "#882255", "#AA4499")
## Apply settings
setwd(dir)
num.robots <- 10
#tol4qualitative <- terrain.colors(num.robots)
df <- read.table(file.name, header=T)
##pdf("/home/volker/Dropbox/mypapers/technical_report_collective/img/blockchain_length.pdf")
pdf("blockchain_fullyconnected.pdf")
par(cex.lab = 1.7, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(df$clock, df$robotep0,
     xlab="Time (sec)", ylab="Blockchain height",
     pch=1, col=tol12qualitative[1], type="b", font.lab=2, cex=1,
     lty=0)
for (i in 2:num.robots) {
    points(df$clock, df[, i + 1],
           col=tol12qualitative[i]
           )    
}
## Plot second strategy
legend(x="bottomright", legend=c("Robot i"),
       col="black",
       cex=2,
       pch=1,
       bty="n")
dev.off()
