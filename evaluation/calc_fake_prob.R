require( tikzDevice )

## Colors
 tol4qualitative=c("#4477AA", "#117733", "#DDCC77", "#CC6677")
## Settings
dir <- "../data/"

## Apply settings
setwd(dir)

## The data frame that holds the data to plot
fake.df <- read.csv("fake_exit_prob.csv")
fake.df2 <- read.csv("fake_exit_prob2.csv")

out.dir <- "/home/volker/Dropbox/mypapers/technical_report_collective/img/"
##pdf(file=paste0(out.dir, "consensustime.pdf"))
tikz(file=paste0(out.dir, "consensustime.tex"))
## Plot first strategy
par(cex.lab = 1.7, cex.axis=1.7, mar=c(9,6,4,2), lwd=3, font.axis=2, font.lab=2, font=2)
plot(fake.df$Ea, fake.df$EN,
     xlab=expression(bolditalic(E[a](0))), ylab=expression(bolditalic(E[N])),
     pch=3, col=tol4qualitative[1], type="b", ylim=c(0, 1), font.lab=2, cex=2,
     lty=2)
abline(h =1, untf = FALSE, lty=2)
abline(h =0, untf = FALSE, lty=2)
## Plot second strategy
lines(fake.df2$Ea, fake.df2$EN,
      pch=2, col=tol4qualitative[2], type="b", cex=2,
      lty=3)
legend(x="bottomright", legend=c("Strategy 1", "Strategy 2"),
       col=tol4qualitative[1:2],
       pch=2:3,
       lty=2:3,
       cex=2,
       bty="n")
dev.off()
