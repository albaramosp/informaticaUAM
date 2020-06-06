#!/bin/bash
#
#$ -S /bin/bash
#$ -cwd
#$ -o productos.out
#$ -j y

# inicializar variables
Minicio=518
Mpaso=64
Mfinal=1542

# numero de hilos
Ninicio=3
Npaso=1
Nfinal=3

echo "Running producto..."
for ((N = Ninicio ; N <= Nfinal ; N += Npaso)); do
	echo "$N hilos..."

	for ((M = Minicio ; M <= Mfinal ; M += Mpaso)); do
		echo "Tamaño $M..."
		sumaProd=0
		sumaProd3=0

		mediaProd=0
		mediaProd3=0

		acel3=0

		for ((i=0 ; i < 5; i++)); do
			echo "Repeticion $i..."
			prodTime=$(./calcularProducto $M | grep 'time' | awk '{print $3}')
			prodTime3=$(./calcularProducto3 $N $M | grep 'time' | awk '{print $3}')
			echo "pasa ejecucion..."
			

			sumaProd=$(echo "scale=9; ($sumaProd + $prodTime)" | bc -l)
			sumaProd3=$(echo "scale=9; ($sumaProd3 + $prodTime3)" | bc -l)

			echo "pasa sumas..."

		done

		mediaProd=$(echo "scale=9; ($sumaProd / 5)" | bc -l)
		mediaProd3=$(echo "scale=9; ($sumaProd3 / 5)" | bc -l)

		echo "pasa medias..."

		acel3=$(echo "scale=9; ($mediaProd / $mediaProd3)" | bc -l)

		echo "pasa acel..."


		echo "$N	$M	$mediaProd	$mediaProd3	" >> ej3_time_best_$N.dat
		echo "$N	$M	$acel3	" >> ej3_acel_best_$N.dat
	done
done
