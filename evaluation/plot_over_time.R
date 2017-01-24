## To clean the received TSV file:
infile <- "volker_num10_red64_blue361.RUN1"

#outfile <- paste0(infile, ".out")
#cmd <- paste("./clean_file.sh", infile)
#system(cmd)

X <- read.table(infile, header=T)
X["allRed"] <- X["exploringRed"] + X["diffusingRed"]
X["allBlue"] <- X["exploringBlue"] + X["diffusingBlue"]

pdf('rplot.pdf')
plot(1:nrow(X), X$allRed, type='l', col='red', ylim = c(0, 12),
     xlab="Time (t)", ylab="Number of robots")
lines(1:nrow(X), X$allBlue, type='l', col='blue')
dev.off()

