NOW=`date +"%d-%m-%Y"`
RACK=3
TEMPLATEFILE=/home/vstrobel/Documents/argdavide/start_byzantine_again.sh
MAIN1=1
MAIN2=3
MAIN3=5

echo "nohup bash ${TEMPLATEFILE} 0 1 1 > nohup_byz_dec1-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} 2 3 2 > nohup_byz_dec2-${NOW}.out 2>&1&
nohup bash ${TEMPLATEFILE} 5 6 3 > nohup_byz_dec3-${NOW}.out 2>&1&" > last_byz_template.txt
