/**
 * @brief Programa que estudia la no linealidad del AES, es decir, que f(A ^ B) /= f(A) ^ f(B). Para ello realiza una serie
 * de tests en los que va generando entradas aleatorias para la funcion F, y comprueba que los resultados sean distintos
 * segun la explicacion que hemos dicho.
 * El programa se llama con la siguiente estructura:
 * /.nonlinear_AES{-C | -D}
 * -C realiza los cálculos con la S-box directa
 * -D realiza los cálculos con la S-box inversa
 * 
 * 
 * @file nonLinear_AES.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 21-11-2020
 * @copyright GNU Public License
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <inttypes.h>
#include "aes.h"


#define TESTS 100000

int main(int argc, char *argv[]) {
	uint8_t a=0, b=0, res=0;/*numeros de 8 bits*/
	uint8_t salidaA=0, salidaB=0, salidaRes=0;
	int i, diferencias=0;
	srand(time(NULL));
	char caja[15];

	if (argc < 2 || argc > 3){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("/.nonLinear_AES {-C | -D}\n");
		printf("-C estudiar la S-box directa\n");
		printf("-D estudiar la S-box inversa\n");
		printf("\n");
	} 
	else {
		if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
			strcpy(caja,"SBOX DIRECTA");
			for(i = 0; i < TESTS; i++){
    
	    		a = rand(); 
				salidaA = (*DIRECT_SBOX)[a]; /* Salida S-boxes */

        		b = rand(); 
				salidaB = (*DIRECT_SBOX)[b];
	
				res= a ^ b;
				salidaRes = (*DIRECT_SBOX)[res];

        		if(salidaRes != (salidaA ^ salidaB)){
            		diferencias++;
        		}
		}	
		}
		else
		{
			strcpy(caja,"SBOX INVERSA");
			for(i = 0; i < TESTS; i++){
		
				a = rand(); 
				salidaA = (*INVERSE_SBOX)[a]; /* Salida S-boxes */

				b = rand(); 
				salidaB = (*INVERSE_SBOX)[b];
		
				res= a ^ b;
				salidaRes = (*INVERSE_SBOX)[res];

				if(salidaRes != (salidaA ^ salidaB)){
					diferencias++;
				}
			}	
		}
	printf("\n-------------COMPROBACION DE LA NO LINEALIDAD DEL AES--------------\n");
	printf("\nLa no linealidad del AES establece que F(A + B) /= F(A) + F(B) \n");
	printf("\nA continuacion se muestra cuantas veces se ha cumplido esto en las pruebas ejecutadas con la caja %s: \n", caja);
    printf("\nDiferencias: %d / %d\n", diferencias, TESTS);	
	}
	return 0;

}
