library(ggplot2)
library(ggthemes)
library(directlabels)
library(grid)


strat2strat.name <- function(strat){

    if (strat == 1) {
        return("DMVD")
    } else if (strat == 2) {
        return("DC")
    } else if (strat == 3) {
        return("DMMD")
    }
}

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

    print(df)
    df[, 'strat.names'] <- as.factor(df[, 'strat.names'])
    p <- ggplot(df, aes(x=difficulty, y=E.Ns, group=strat.names)) +
        geom_line(aes(colour = strat.names), size=1.1) +
        geom_point(aes(colour = strat.names, shape = strat.names), size=3) +
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
        coord_cartesian(xlim=c(min(difficulty), max(difficulty))) + 

    #+ xlim(c(, max(difficulty)))

    base_breaks_x(seq(34, 48, 2)) + 
    base_breaks_y(seq(0.0, 1, 0.1)) + 
        
    expand_limits(x = 1.05)

    p <- direct.label(p, list(dl.trans(x=x+0.2, y=y),
                              list("angled.boxes", cex=1.0)))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
        
    #p <- direct.label(p, "last.qp")
    ggsave(paste0(report.dir, out.name))
}

plot.exit.prob.gg.byz <- function(df, xlab, ylab, out.name) {

    print(df)
    df[, 'strat.names'] <- as.factor(df[, 'strat.names'])
    p <- ggplot(df, aes(x=num.byz, y=E.Ns, group=strat.names)) +
        geom_line(aes(colour = strat.names), size=1.1) +
        geom_point(aes(colour = strat.names, shape = strat.names), size=3) +
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

    base_breaks_x(seq(min(num.byz), max(num.byz), 1)) + 
    base_breaks_y(seq(0.0, 1, 0.1)) + 
        
    expand_limits(x = 1.05)
    #geom_dl(aes(colour = strat.names, label=strat.names),
    #        method="lasso.labels",
    #        position=list(dl.trans(x=x-0.2, y=y))

    p <- direct.label(p, list(dl.trans(x=x, y=y),
                              list("angled.boxes", cex=1.0)))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
        
    #p <- direct.label(p, "last.qp")
    ggsave(paste0(report.dir, out.name))
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
plot.consensus.time.gg.byz <- function(df, xlab, ylab, out.name) {

    df[, 'strat.names'] <- as.factor(df[, 'strat.names'])
    p <- ggplot(df, aes(x=num.byz, y=consensus.time / 10, group=strat.names)) +
        geom_line(aes(colour = strat.names), size=1.1) +
        geom_point(aes(colour = strat.names, shape = strat.names), size=3) +
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
#        coord_fixed()
#    base_breaks_x(seq(0.5, 1, 0.1)) +
    base_breaks_x(seq(0, 10, 1)) +
#        base_breaks_x(seq(0, 20, 5)) +
        base_breaks_y(seq(0, 140, 20))# + expand_limits(x=25)


    p <- direct.label(p, list(dl.trans(x=x, y=y),
                              list("angled.boxes", cex=1.0)))
    
   # p <- direct.label(p, list(dl.trans(x=x-2.5, y=y+0.4), "last.qp"))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
    
    ggsave(paste0(report.dir, out.name))
    }

## Plot conensus time
plot.consensus.time.gg <- function(df, xlab, ylab, out.name) {

    print("In function now (plot.consensus.time.gg)")
    print(df)
    
    df[, 'strat.names'] <- as.factor(df[, 'strat.names'])
    p <- ggplot(df, aes(x=difficulty, y=consensus.time / 10, group=strat.names)) +
        geom_line(aes(colour = strat.names), size=1.1) +
        geom_point(aes(colour = strat.names, shape = strat.names), size=3) +
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
#        coord_fixed()
#    base_breaks_x(seq(0.5, 1, 0.1)) +
    base_breaks_x(seq(min(difficulty), max(difficulty), 2)) +
#        base_breaks_x(seq(0, 20, 5)) +
        base_breaks_y(seq(0, 140, 20))# + expand_limits(x=25)


    p <- direct.label(p, list(dl.trans(x=x, y=y),
                              list("angled.boxes", cex=1.0)))
    
   # p <- direct.label(p, list(dl.trans(x=x-2.5, y=y+0.4), "last.qp"))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
    
    ggsave(paste0(report.dir, out.name))
    }




## Plot blockchain height
## Save as function above just for the blockchain height
plot.bc.height.gg <- function(df, xlab, ylab, out.name) {

    df[, 'strategy'] <- as.factor(df[, 'strategy'])
    p <- ggplot(df, aes(x=difficulty, y=bc.height, group=strategy)) +
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
#        coord_fixed()
        base_breaks_x(seq(0.5, 1, 0.1)) +
#        base_breaks_x(seq(0, 20, 5)) +
        base_breaks_y(seq(0, 140, 20))# + expand_limits(x=25)


    p <- direct.label(p, list(dl.trans(x=x+0.1, y=y),
                              list("angled.boxes", cex=1.0)))
    
   # p <- direct.label(p, list(dl.trans(x=x-2.5, y=y+0.4), "last.qp"))

    ## Code to turn off clipping
    gt1 <- ggplotGrob(p)
    gt1$layout$clip[gt1$layout$name == "panel"] <- "off"
    grid.draw(gt1)
    
    ggsave(paste0(report.dir, "ggplot_bcheight.pdf"))
    }
