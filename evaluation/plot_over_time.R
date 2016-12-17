## To clean the received TSV file:
infile <- "4.RUN1"
outfile <- paste0(infile, ".out")
cmd <- paste("./clean_file.sh", infile)
system(cmd)

X <- read.table(outfile, header=T)
X["allRed"] <- X["exploringRed"] + X["diffusingRed"]
X["allBlue"] <- X["exploringBlue"] + X["diffusingBlue"]

plot(1:nrow(X), X$allRed, type='l', col='red', ylim = c(0, 12),
     xlab="Time (t)", ylab="Number of robots")
lines(1:nrow(X), X$allBlue, type='l', col='blue')


