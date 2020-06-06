#!/bin/bash
#
#$ -S /bin/bash
#$ -cwd
#$ -o simple.out
#$ -j y

# inicializar variables
Ninicio=16592
Npaso=64
Nfinal=16912
fDAT=slow_fast_media2.dat

# borrar el fichero DAT y el fichero PNG
rm -f $fDAT

# generar el fichero DAT vacío
touch $fDAT

echo "Running slow and fast..."
for ((N = Ninicio ; N <= Nfinal ; N += Npaso)); do
	echo "N: $N / $Nfinal..."
	sumaSlow=0
	sumaFast=0
	
	for ((i=0 ; i < 15; i++)); do
		slowTime=$(./slow $N | grep 'time' | awk '{print $3}')
		slowTime2=$(./slow $N+$Npaso | grep 'time' | awk '{print $3}')
		fastTime=$(./fast $N | grep 'time' | awk '{print $3}')
		fastTime2=$(./fast $N+$Npaso | grep 'time' | awk '{print $3}')
		
		sumaSlow=$(echo "scale=9; ($sumaSlow + $slowTime)" | bc -l)
		sumaFast=$(echo "scale=9; ($sumaFast + $fastTime)" | bc -l)
	done
	
	mediaSlow=$(echo "scale=9; ($sumaSlow / 20)" | bc -l)
	mediaFast=$(echo "scale=9; ($sumaFast / 20)" | bc -l)
	
	echo "$N    $mediaSlow  $mediaFast" >> $fDAT
done
