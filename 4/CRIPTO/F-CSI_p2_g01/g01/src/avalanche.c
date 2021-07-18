/**
 * @brief Programa que estudia el efecto avalancha del DES, el cual indica que cualquier bit
 * de la salida de la s-box debe cambiar con probabilidad 0'5 si cambia uno de la entrada. Para ello realiza
 * una cantidad de tests (cuanto mayor sea esta cantidad, mejores serán los resultados) en los que, para
 * cada uno, genera una entrada aleatoria para la SBOX y va complementando cada uno de los bits de entrada, 
 * comprobando después qué cambios ocurren a la salida. Este cambio puede comprobarse tanto con un bit a 1
 * como a 0, es indiferente según la teoría. Finalmente, calcula la probabilidad de cambio que ha tenido cada bit.
 * Muestra este resultado para comprobar que, efectivamente, es próxima al 50%.
 * El programa se llama con el comando /.avalanche
 * 
 * @file avalanche.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 18-11-2020
 * @copyright GNU Public License
 */

#include <time.h>
#include <math.h>
#include "funcionesDES.h"

#define TESTS 100000 /* Esta es buena cantidad, con menos no se ve tanto el efecto */

int main (){
	uint64_t entrada=0, salida=0;
	uint64_t maskCompl=0x0000041041041041;
	uint64_t mask48 = 0x0000FFFFFFFFFFFF;
	int i, j, k;
	int bitsCambiados[32]; /* Contar los cambios producidos en cada bit de la salida de la sbox */

	srand(time(NULL));

	/* Inicializar los contadores */
	for (i = 0; i < 32; i++){
		bitsCambiados[i] = 0;
	}

	for (i = 0; i < TESTS; i++){
		entrada = rand(); 
		entrada <<= 32; /* Un int son 4B = 32b */
		entrada |= rand(); /* Ahora tenemos 64b aleatorios */
		entrada &= mask48; /* Nos quedamos solo con 48b de los 64b que teníamos */
		
		/* Complementamos uno de los 6 bits de la entrada de cada sbox cada vez */
		for (j = 0; j < 6; j++){
			salida = sbox(entrada ^ maskCompl); /* La XOR complementa */

			/* Comprobamos qué bits de la salida de la sbox han cambiado */
			for (k = 0; k < 32; k++){
				if (((salida >> k) & 0x1) == 1){
					bitsCambiados[k]++;
				}
			}

			maskCompl <<= 1; /* Actualiza máscara para próxima complementación */
		}
	}

	printf("\n-------------COMPROBACION DEL EFECTO AVALANCHA DEL DES--------------\n");
	printf("\nEl Strict Avalanche Criterion establece que para todo i,j, P(ci=1 | /bj) = P(ci=0 | /bj) = 0.5, \n");
	printf("siendo ci un bit de la salida de la SBOX y bj uno de la entrada. ");
	printf("\nA continuacion se muestra la probabilidad de cambio de cada bit tras las pruebas ejecutadas: \n");
	for(i = 0; i < 32; i++) {
		printf("Bit %d --> %.2f%%\n", i+1, (float)bitsCambiados[i] / 6 * 100 / TESTS);
	}

	return 0;
}