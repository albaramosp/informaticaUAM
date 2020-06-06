#!/bin/bash
#
#$ -S /bin/bash
#$ -cwd
#$ -o simple.out
#$ -j y

# inicializar variables
Ninicio=1792
Npaso=64
Nfinal=2048

touch aux.dat
touch out.dat

echo "Running producto and productoTraspuesta..."
for ((N = Ninicio ; N <= Nfinal ; N += Npaso)); do
	echo "N: $N / $Nfinal..."
	sumaFails1Prod=0
	sumaFails1Tras=0
	sumaFails2Prod=0
	sumaFails2Tras=0
	
	for ((i=0 ; i < 1; i++)); do
		valgrind --tool=cachegrind --cachegrind-out-file=aux.dat ./calcularProducto $N
		tail -n 1 aux.dat > out.dat
		prodFails1=$(awk '{print $6}' out.dat)
		prodFails2=$(awk '{print $9}' out.dat)
		valgrind --tool=cachegrind --cachegrind-out-file=aux.dat ./calcularProducto $N+$Npaso

		valgrind --tool=cachegrind --cachegrind-out-file=aux.dat ./calcularProductoTraspuesta $N
		tail -n 1 aux.dat > out.dat
		trasFails1=$(awk '{print $6}' out.dat)
		trasFails2=$(awk '{print $9}' out.dat)
		valgrind --tool=cachegrind --cachegrind-out-file=aux.dat ./calcularProductoTraspuesta $N+$Npaso

		sumaFails1Prod=$(echo "scale=9; ($prodFails1 + $sumaFails1Prod)" | bc -l)
		sumaFails1Tras=$(echo "scale=9; ($prodFails2 + $sumaFails1Tras)" | bc -l)
		sumaFails2Prod=$(echo "scale=9; ($trasFails1 + $sumaFails2Prod)" | bc -l)
		sumaFails2Tras=$(echo "scale=9; ($trasFails2 + $sumaFails2Tras)" | bc -l)
		
	done
	
	mediaFails1Prod=$(echo "scale=9; ($sumaFails1Prod / 1)" | bc -l)
	mediaFails1Tras=$(echo "scale=9; ($sumaFails1Tras / 1)" | bc -l)
	mediaFails2Prod=$(echo "scale=9; ($sumaFails2Prod / 1)" | bc -l)
	mediaFails2Tras=$(echo "scale=9; ($sumaFails2Tras / 1)" | bc -l)

	echo "$N	$mediaFails1Prod	$mediaFails2Prod	$mediaFails1Tras	$mediaFails2Tras" >> mult.dat
done

rm -rf aux.dat
rm -rf out.dat
