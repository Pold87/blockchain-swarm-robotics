TEMPLATE='experiments/epuck_EC_locale_template.argos'
OUTFILE='experiments/epuck.argos'
BASEDIR='/home/volker/Downloads/code/code/argos_simulations/Epuck/controllers/epuck_environment_classification/'
#BASEDIR='/home/vstrobel/Documents/argdavide/controllers/epuck_environment_classification/'
RADIX="volker_trial_new"
NUMROBOTS=2
R0=$(expr $NUMROBOTS / 2)
B0=$(expr $NUMROBOTS / 2)
PERCENT_RED=48
PERCENT_BLUE=$(expr 100 - $PERCENT_RED)

REPETITIONS=10

for i in `seq 1 $REPETITIONS`; do

    # Create template
sed -e "s|BASEDIR|$BASEDIR|g" -e "s|RADIX|$RADIX$i|g" -e "s|NUMROBOTS|$NUMROBOTS|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_RED|g" -e "s|PERCENT_BLUE|$PERCENT_BLUE|g" $TEMPLATE > $OUTFILE

    # Start experiment
argos3 -c $OUTFILE
    
done

