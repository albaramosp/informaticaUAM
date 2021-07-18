#include "funcionesDES.h"

uint64_t permutar(uint64_t input, const unsigned short *tabla, int tamanio_inicial, int tamanio_final){	
	int i, desplazamiento;	
	uint64_t bit1;
	uint64_t mascara;
	uint64_t output = 0x0;

	/*si hay menos bits en lo que entra que en la tabla*/
	if (tamanio_inicial < tamanio_final){
		/*rellenamos con ceros a la derecha el tamanio de la entrada*/
		input <<= (tamanio_final - tamanio_inicial);
		/*ahora tamanio_inicial tiene la longitud de la tabla*/
		tamanio_inicial += (tamanio_final - tamanio_inicial);
	}

	bit1 = (uint64_t)1 << (tamanio_inicial -1);

	for (i = 0; i < tamanio_final; i++){
		/*el indice de la tabla nos dice a donde hay que ir*/
		desplazamiento = (int) tabla[i];
		mascara = bit1 >> (desplazamiento - 1);
		desplazamiento = desplazamiento -i -1;
		if(desplazamiento > 0){

			output = output | ((input & mascara) << (desplazamiento));
			
		} else {
			output = output | ((input & mascara) >> (-desplazamiento));
		}
	}
	
	/* Si deben haber mas bits que los que llegaron, rellenamos con ceros */
	if (tamanio_inicial > tamanio_final){
		output >>= (tamanio_inicial - tamanio_final);
	}

	return output;
}

uint64_t* generarSubclaves (uint64_t k){
	int i;
	uint64_t k_perm, Co, Do;
	uint64_t *subclaves;

	/*reserva memoria para las 16 subclaves*/
	subclaves = (uint64_t *) malloc(16 * sizeof(uint64_t));
	
	/*la clave de 64 bits se permuta de acuerdo con la tabla PC-1 y obtenemos la permutación de 56 bits*/
	k_perm = permutar(k, PC1, 64, 56);
	/*printf("\nK+ = %lx", k_perm);*/
	
	/*Dividir la clave permutada de 56 bits en 2 mitades de 28 bits*/
	Co = (k_perm & 0xFFFFFFF0000000) >> 28; 
	Do = k_perm & 0x0000000FFFFFFF;
	/*printf("\nCo = %lx", Co);
	printf("\nDo = %lx", Do);*/
	
	/* Generacion de subclaves */
	for(i = 0; i < 16; i++) {
		/* Desplaza 1 o 2 bits hacia la izda */
		Co = 0x0000000FFFFFFF & ((Co << ROUND_SHIFTS[i]) | (Co >> (28 - ROUND_SHIFTS[i])));
		Do = 0x0000000FFFFFFF & ((Do << ROUND_SHIFTS[i]) | (Do >> (28 - ROUND_SHIFTS[i])));


		/*printf("\nCo = %lx", Co);
		printf("\nDo = %lx", Do);*/
		/*en la última Co = f0ccaaf y Do = 556678f*/
		
		/* Concatenamos los pares anteriores para obtener la clave de 56 bits de esa ronda */
		k_perm = Co << 28 | Do; 
			
		/*Se aplica la tabla de permutación PC2 de 48 bits a cada clave de 56 bits*/
		subclaves[i] = permutar(k_perm, PC2, 56, 48); 
		/*printf("\nK%d = %lx", i, subclaves[i]);*/
		/*ultima clave es cb3d8b0e17f5*/
	}

	return subclaves;
}

uint64_t sbox(uint64_t input){
	int i;
	uint64_t fila, columna, a1, a6, entradaBox, salidaBox=0x0;
	uint64_t mascara6 = 0xFC0000000000;
	uint64_t mascaraA1 = 0x800000000000;
	uint64_t mascaraA6 = 0x040000000000;
	uint64_t mascaraColumna = 0x7C0000000000;


	for (i = 0; i < NUM_S_BOXES; i++){
		/* Llega a1,a2,a3,a4,a5,a6 */
		entradaBox = mascara6 & input;

		/* Fila = valor a1a6 en decimal */
		a1 = mascaraA1 & entradaBox;
		a6 = mascaraA6 & entradaBox;

		/* Para tener a1 y a6 juntos */
		a1 >>= 4; 
		fila = a1 | a6;
		fila >>= (6 * (7 - i) );

		/* Columna = valor a2a3a4a5 en decimal */
		columna = mascaraColumna & entradaBox;
		columna >>= 6 * (7 - i) + 1;

		/*printf("\nIndexar a SBOX[%lx][%lx]", fila, columna);*/
		salidaBox |= S_BOXES[i][fila][columna];
		/*printf("Salida de SBOX = %lx\n", salidaBox);*/
		salidaBox <<= 4;  /* Dejar hueco para la OR siguiente */

		/* Recalcular mascaras de 6 bits, fila y columna */
		mascara6 >>= 6;
		mascaraA1 >>= 6;
		mascaraA6 >>= 6;
		mascaraColumna >>= 6;
	}

	salidaBox >>= 4; /* Deshacer hueco OR de la ultima iteracion */

	return salidaBox;
}

uint64_t des(uint64_t mensaje, uint64_t k, int cifrar){
	uint64_t resultado=0;
	uint64_t izda=0, dcha=0, xor=0, f=0;
	uint64_t *subclaves=NULL, *subclavesInv=NULL;
	int i;

	/* Generar subclaves para las 16 rondas */
	subclaves = generarSubclaves(k);

	/* Si se está descifrando se les da la vuelta */
	if (cifrar == 0){
		subclavesInv = (uint64_t *)malloc(16 * sizeof(uint64_t));
		
		for (i = 0; i < 16; i++){
			subclavesInv[i] = subclaves[15 - i];
		}

		/* Liberar las claves iniciales para cifrado */
		free(subclaves);

		/* Sustituirlas por las inversas para descifrado */
		subclaves = subclavesInv;
	}

	/* Permutación inicial (IP) del mensaje */
	resultado = permutar(mensaje, IP, 64, 64);
	/*printf("\nMensaje tras IP: %lx", resultado);*/

	/* Dividimos en parte izda y parte dcha */
	izda = (0xFFFFFFFF00000000 & resultado) >> 32;
	dcha = 0x00000000FFFFFFFF & resultado;

	/* 16 rondas del DES */
	for (i = 0; i < 16; i++){
		/*printf("\nLi = %lx", izda);
		printf("\nRi = %lx", dcha);*/
		/* Comienzo funcion F */
		/* Expansion de parte dcha para xor y S-Box de 48b */
		f = permutar(dcha, E, 32, 48);

		/* XOR con la clave */
		f ^= subclaves[i];

		/* Salida S-Box */
		f = sbox(f);

		/* Permutacion */
		f = permutar(f, P, 32, 32);
		
		/* Fin funcion F */
		xor = izda ^ f;

		izda = dcha;
		dcha = xor;
	}

	/* Swap final */
	resultado = dcha << 32;
	resultado |= izda;

	/* Permutación final (IP_inv) del mensaje */
	resultado = permutar(resultado, IP_INV, 64, 64);
	/*printf("\nMensaje tras IP_INV: %lx", resultado);*/
	free(subclaves);

	return resultado;
}

int findParity(uint64_t n) {
   int count = 0;
   uint64_t temp = n;

   while (temp>=2) {
      if(temp & 1)    //when LSb is 1, increase count
         count++;
      temp = temp >> 1;    //right shift number by 1 bit
   }      
   return (count % 2)?1:0;
}
