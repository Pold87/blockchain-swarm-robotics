NOW=`date +"%d-%m-%Y"`
RACK=3
TEMPLATEFILE=$HOME/Documents/argdavide/start_experiment1.sh
#TEMPLATEFILE=$HOME/Documents/argdavide/start_experiment2.sh

echo "nohup bash ${TEMPLATEFILE} 0 1 2 > nohup_experiment1_dec1a-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} 2 3 1 > nohup_experiment1_dec1b-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} 5 7 1 > nohup_experiment1_dec1c-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} 8 9 1 > nohup_experiment1_dec1d-${NOW}.out 2>&1&" > last_template.txt
