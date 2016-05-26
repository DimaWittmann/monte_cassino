i="0"

while [ $i -lt 100 ]
do 
	insmod tasklet.ko
	rmmod tasklet.ko
	i=$[$i+1]
done
