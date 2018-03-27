NOW=`date +"%d-%m-%Y"`
TEMPLATEFILE=start_experiment1.sh

echo "nohup bash ${TEMPLATEFILE} 0 0 2 > nohup_experiment1_dec1a-${NOW}.out 2>&1&" > last_template.txt
