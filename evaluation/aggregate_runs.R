## df <- data.frame(Runs=numeric(0),
##                  ExitTime=numeric(0),
##                  Whites=numeric(0),
##                  Greens=numeric(0),
##                  Blacks=numeric(0))
max.byzantine <- 10
data.base <- "/home/volker/localargdavide/data/"
data.dir <- "experiment1_decision2-node0-byzantine-THREAD-extra2-15-10-2017"


create.df <- function(data.dir, max.trials=30) {
    for (i in 0:max.trials) {
        for (b in 0:max.byzantine) {
            trials.name <- sprintf("%s/%s/num20_black34_byz%d_run%d.RUNS",
                                   data.base, data.dir, b, i)
        
            if (file.exists(trials.name)) {    
                X <- read.table(trials.name, header=T)
                if (nrow(X) != 0){ 
                X$num.byz = b
                if ((i == 0) && ((b == 0))) {
                    df <- X
                } else  {
                    df <- rbind(df, X)
                }
                }
            }           
        }
    }    
    return(df)    
}


df <- create.df("experiment1_decision2-node0-byzantine-THREAD-extra2-15-10-2017")

print(df[order(df$num.byz),])
