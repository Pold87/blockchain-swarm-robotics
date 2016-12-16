TEMPLATE='epuck_EC_locale_template.argos'
BASEDIR='/home/volker/Downloads/code/code/argos_simulations/Epuck/controllers/'
#BASEDIR='/home/vstrobel/Documents/argdavide/Epuck/controllers/'
RADIX="volker_trial"
NUMROBOTS=10
R0=$(expr $NUMROBOTS / 2)
B0=$(expr $NUMROBOTS / 2)
PERCENT_RED=80
PERCENT_BLUE=$(expr 100 - $PERCENT_RED)

REPETITIONS=10

for i in `seq 1 $REPETITIONS`; do

    # Create template
sed -e "s|BASEDIR|$BASEDIR|g" -e "s|RADIX|$RADIX_$i|g" -e "s|NUMROBOTS|$NUMROBOTS|g" -e "s|R0|$R0|g" -e "s|B0|$B0|g" -e "s|PERCENT_RED|$PERCENT_RED|g" -e "s|PERCENT_BLUE|$PERCENT_BLUE|g" $TEMPLATE

    
done

