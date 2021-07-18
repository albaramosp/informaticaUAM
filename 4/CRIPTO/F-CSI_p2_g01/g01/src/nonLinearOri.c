/**
 * @brief Programa que estudia la no linealidad del DES, es decir, que f(A ^ B) /= f(A) ^ f(B). Para ello realiza una serie
 * de tests en los que va generando entradas aleatorias para la funcion F, y comprueba que los resultados sean distintos
 * segun la explicacion que hemos dicho.
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
	uint64_t salidaA=0, salidaB=0, salidaRes=0;
	uint64_t mask48 = 0x0000FFFFFFFFFFFF;
	int i, diferencias=0;

	srand(time(NULL));

	for(i = 0; i < TESTS; i++){
        a = rand(); 
		a <<= 32; /* Un int son 4B = 32b */
		a |= rand(); /* Ahora tenemos 64b aleatorios */
		a &= mask48; /* Nos quedamos solo con 48b de los 64b que teníamos */

        b = rand(); 
		b <<= 32; 
		b |= rand(); 
		b &= mask48; 

        res = rand(); 
		res <<= 32; 
		res |= rand(); 
		res &= mask48;

		res= a ^ b;

		salidaA = sbox(a);
		salidaB = sbox(b);
		salidaRes = sbox(res);

        if(salidaRes != (salidaA ^ salidaB)){
            diferencias++;
        }
	}	

    printf("\n-------------COMPROBACION DE LA NO LINEALIDAD DEL DES--------------\n");
	printf("\nLa no linealidad del DES establece que F(A + B) /= F(A) + F(B) \n");
	printf("\nA continuacion se muestra cuantas veces se ha cumplido esto en las pruebas ejecutadas: \n");
    printf("\nDiferencias: %d / %d\n", TESTS, diferencias);
}