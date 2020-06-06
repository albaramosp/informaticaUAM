#!/bin/bash
#
#$ -S /bin/bash
#$ -cwd
#$ -o ej2.out
#$ -j y

# numero de hilos
Ninicio=1
Npaso=1
Nfinal=4

# tam vector
Minicio=3024407
Mpaso=20700000
Mfinal=210024407

echo "Running producto escalar serie and parallel2..."
for ((N = Ninicio ; N <= Nfinal ; N += Npaso)); do
	echo "$N hilos..."

  for ((M = Minicio ; M <= Mfinal ; M += Mpaso)); do
  		echo "Tamaño $M..."
		sumaPar=0
		sumaSerie=0

		for ((i=0 ; i < 5; i++)); do
			echo "Repeticion $i..."
			parTime=$(./pescalar_par2 $N $M | grep 'Tiempo' | awk '{print $2}')
			serieTime=$(./pescalar_serie $M | grep 'Tiempo' | awk '{print $2}')

			sumaPar=$(echo "scale=9; ($sumaPar + $parTime)" | bc -l)
			sumaSerie=$(echo "scale=9; ($sumaSerie + $serieTime)" | bc -l)

		done

		mediaSerie=$(echo "scale=9; ($sumaSerie / 5)" | bc -l)
		mediaPar=$(echo "scale=9; ($sumaPar / 5)" | bc -l)

		echo "$N	$M	$mediaSerie	$mediaPar	" >> ej2.dat
	done
done
