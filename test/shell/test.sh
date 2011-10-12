index=0;max=0;min=0;
for value in `cat data`; 
do 
	if [ $index -eq 0 ]; then max=$value; min=$value; index=1; 
	fi
	if [ $value -gt $max ]; then max=$value; 
	fi
	if [ $value -lt $min ]; then  min=$value; 
	fi
done
echo max=$max
echo min=$min
