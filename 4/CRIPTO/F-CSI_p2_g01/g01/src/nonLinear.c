/**
 * @brief Programa que estudia la no linealidad del DES, es decir, que f(A ^ B) /= f(A) ^ f(B). Para ello realiza una serie
 * de tests en los que va generando entradas aleatorias para la funcion F, y comprueba que los resultados sean distintos
 * segun la explicacion que hemos dicho.
 * La funcion F consta de una Expansion de los 32b que entrarian de la parte derecha de la entrada de la ronda del DES, 
 * luego una XOR con la clave de dicha ronda, luego las S-BOXES y finalmente una Permutación.
 * El programa se llama con el comando /.nonLinear
 * 
 * @file nonLinear.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 21-11-2020
 * @copyright GNU Public License
 */

#include <time.h>
#include <math.h>
#include "funcionesDES.h"

#define TESTS 100000

int main() {
	uint64_t a=0, b=0, res=0;
	uint64_t salidaA=0, salidaB=0, salidaRes=0, clave=0;
	int i, diferencias=0;

	srand(time(NULL));

	for(i = 0; i < TESTS; i++){
		clave = rand(); 
		clave <<= 32; /* Un int son 4B = 32b */
		clave |= rand(); /* Ahora tenemos 64b aleatorios */

        a = rand(); 
		a <<= 32; 
		a |= rand(); /* 64 bits serían como la entrada de la ronda del DES */
		a = 0x00000000FFFFFFFF & a; /* Parte derecha */
		a = permutar(a, E, 32, 48); /* Expansion */
		a ^= clave; /* XOR con la clave de esa ronda */
		salidaA = sbox(a); /* Salida S-boxes */
		salidaA = permutar(salidaA, P, 32, 32); /* Permutacion de dicha salida --> F(A) */

        b = rand(); 
		b <<= 32; 
		b |= rand(); 
		b = 0x00000000FFFFFFFF & b;
		b = permutar(b, E, 32, 48);
		b ^= clave;
		salidaB = sbox(b);
		salidaB = permutar(salidaB, P, 32, 32); /* --> F(B) */

		res= a ^ b;
		res = 0x00000000FFFFFFFF & res;
		res = permutar(res, E, 32, 48);
		res ^= clave;
		salidaRes = sbox(res);
		salidaRes = permutar(salidaRes, P, 32, 32); /* --> F(A + B) */

        if(salidaRes != (salidaA ^ salidaB)){
            diferencias++;
        }
	}	

    printf("\n-------------COMPROBACION DE LA NO LINEALIDAD DEL DES--------------\n");
	printf("\nLa no linealidad del DES establece que F(A + B) /= F(A) + F(B) \n");
	printf("\nA continuacion se muestra cuantas veces se ha cumplido esto en las pruebas ejecutadas: \n");
    printf("\nDiferencias: %d / %d\n", TESTS, diferencias);
}