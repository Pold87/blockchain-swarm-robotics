## Settings
trials.name <- "volker_trial_new"
dir <- "../data/"
num.trials <- 
num.robots <- 10
ground.truth <- "Blues"

## Apply settings
setwd(dir)

## The data frame that holds the data to plot
fake.df <- read.csv("fake_exit_prob.csv")

for (d in difficulties) {

    ## For all difficulties
    c <- 0
    
    ## For all trials
    for (i in 1:num.trials) {

    
        f <- paste0(trials.name, i, ".RUNS")
        X <- read.table(f, header=T)

        if (X[1, ground.truth] == num.robots) {
            c <- c + 1
        }
    }

    ## E.N is the exit probability
    E.N <- c / num.trials

    print(paste("The exit probability is", E.N))
    
}

