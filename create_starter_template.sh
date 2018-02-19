NOW=`date +"%d-%m-%Y"`
RACK=3
TEMPLATEFILE=/home/vstrobel/Documents/argdavide/start_redo_diffs_eth.sh
MAIN1=1
MAIN2=3
MAIN3=5


echo "nohup bash ${TEMPLATEFILE} ${MAIN1} 2 1 > nohupredodiffs_dec1-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} ${MAIN2} 4 2 > nohupredodiffs_dec2-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} ${MAIN3} 6 3 > nohupredodiffs_dec3-${NOW}.out 2>&1&" > last_template.txt
