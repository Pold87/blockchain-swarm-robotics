INFILE=$1
OUTFILE=$INFILE.out

# Get first line
line=$(head -n 1 $INFILE)

# Delete every second line
tail -n +2 $INFILE | awk 'NR % 2 == 1' > $OUTFILE 

# Add back header
sed -i "1s/^/$line\n/g" $OUTFILE

