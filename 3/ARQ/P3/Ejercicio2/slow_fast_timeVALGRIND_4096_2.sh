#!/bin/bash
#
#$ -S /bin/bash
#$ -cwd
#$ -o simple.out
#$ -j y

Ninicio=5392
Npaso=64
Nfinal=5584
Tinicio=4096
Tpaso=1024
Tfinal=4096
L=8388608

export PATH=$PATH:/share/apps/tools/valgrind/bin
export VALGRIND_LIB=/share/apps/tools/valgrind/lib/valgrind
touch aux.dat
touch out.dat

echo "Running slow and fast..."
# bucle para N desde P hasta Q 
#for N in $(seq $Ninicio $Npaso $Nfinal);
for T in $(seq $Tinicio $Tpaso $Tfinal); do
	for ((N = Ninicio ; N <= Nfinal ; N += Npaso)); do
	suma1Slow=0
    	suma1Fast=0
    	suma2Slow=0
    	suma2Fast=0
    	
    	echo "T: $T"
		echo "N: $N / $Nfinal..."
		for ((i=0 ; i < 12; i++)); do
		    valgrind --tool=cachegrind --I1=$T,1,64 --D1=$T,1,64 --LL=$L,1,64 --cachegrind-out-file=aux.dat ./slow $N
		    tail -n 1 aux.dat > out.dat
		    slowTime1=$(awk '{print $6}' out.dat)
		    slowTime2=$(awk '{print $9}' out.dat)
		    valgrind --tool=cachegrind --I1=$T,1,64 --D1=$T,1,64 --LL=$L,1,64 --cachegrind-out-file=aux.dat ./slow $N+$Npaso
		    
		    valgrind --tool=cachegrind --I1=$T,1,64 --D1=$T,1,64 --LL=$L,1,64 --cachegrind-out-file=aux.dat ./fast $N
		    tail -n 1 aux.dat > out.dat # ultima linea interesa: la metemos en fichero auxiliar
		    fastTime1=$(awk '{print $6}' out.dat)
		    fastTime2=$(awk '{print $9}' out.dat)
		    valgrind --tool=cachegrind --I1=$T,1,64 --D1=$T,1,64 --LL=$L,1,64 --cachegrind-out-file=aux.dat ./fast $N+$Npaso

			suma1Slow=$(echo "scale=9; ($slowTime1 + $suma1Slow)" | bc -l)
			suma1Fast=$(echo "scale=9; ($fastTime1 + $suma1Fast)" | bc -l)
			suma2Slow=$(echo "scale=9; ($slowTime2 + $suma2Slow)" | bc -l)
			suma2Fast=$(echo "scale=9; ($fastTime2 + $suma2Fast)" | bc -l)
		done	
		
		media1Slow=$(echo "scale=9; ($suma1Slow / 12)" | bc -l)
		media1Fast=$(echo "scale=9; ($suma1Fast / 12)" | bc -l)
		media2Slow=$(echo "scale=9; ($suma2Slow / 12)" | bc -l)
		media2Fast=$(echo "scale=9; ($suma2Fast / 12)" | bc -l)

		echo "$N	$media1Slow     $media2Slow     $media1Fast     $media2Fast" >> cache_2_$T.dat
	done
done

rm -rf aux.dat
rm -rf out.dat


