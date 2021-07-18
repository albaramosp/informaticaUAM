/**
 * @brief Programa que calcula las S-boxes para el AES, tanto la directa como la inversa
 * utilizando el algoritmo de Euclides extendido.
 * El programa se llama con la siguiente estructura:
 * /.SBOX_AES{-C | -D}[-o fileout]
 * -C calcular la S-box directa
 * -D calcular la S-box inversa
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file SBOX_AES.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 29-10-2020
 * @copyright GNU Public License
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>


/*desplaza los bits indicados por shift a derecha o izquierda*/
#define DESP(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

/*calcula la diferencia entre dos longitudes de bits. La constante 32 no nos sirve para nada*/
/*clz va comparando bits desde el más significativo hasta que encuentra uno distinto de cero*/
/*clz:recuento de ceros a la izquierda*/
#define bitlength(x) (32 - __builtin_clz(x))

uint8_t inverso(uint8_t x){

    uint16_t u1 = 0, r0 = 0x11b, v1 = 1, r1 = x;

	/*mientras haya resto*/
    while (r1 != 0) {
        uint16_t t1 = u1, t2 = r0;
	
		/*calcula donde está el bit más significativo de r1*/
        int8_t q = bitlength(r0) - bitlength(r1);
		
        if (q >= 0) {
            t1 ^= v1 << q;
            t2 ^= r1 << q;
        }
        u1 = v1; 
		r0 = r1;
        v1 = t1; 
		r1 = t2;
    }

    if (u1 >= 0x100){ 
		u1 ^= 0x11b;
	}

    return u1;
}

/*El S-box asigna una entrada de 8 bits, c , a una salida de 8 bits, s = S ( c )
 interpretados como polinomios sobre GF(2)*/ 
void sbox_directa(uint8_t sbox_d[256]) {
	uint8_t b;
	uint8_t i;
	int cont=1;

	for(i=1;cont<256;i++,cont++){
		/*se procede a obtener el inverso*/
		b = inverso(i);

		/*Transformación afín: es la suma de múltiples rotaciones del byte como vector, donde la suma es la operación XOR y se hace xor 
		con la constante M(x)=63 en hex*/
		sbox_d[i] = b ^ DESP(b, 1) ^ DESP(b, 2) ^ DESP(b, 3) ^ DESP(b, 4) ^ 0x63;
		/*esto anterior es equivalente a si = b xor b(i+4)mod8 xor b(i+5)mod8 xor b(i+6)mod8 xor b(i+7)mod8 xor Ci*/

	}
	/*se considera que inverso de '00' es '00' por lo tanto s[0]=C */
	sbox_d[0] = 0x63;

return;

}

/*La sbox_inversa es la sbox_directa ejecutada al reves*/
void sbox_inversa(uint8_t sbox_i[256]) {
	uint8_t b;
	uint8_t i;
	int cont=1;

	for(i=1;cont<256;i++,cont++){
		/*Transformación afín: es la suma de múltiples rotaciones del byte como vector, donde la suma es la operación XOR y se hace xor 
		con la constante M(x)=05 en hex*/
		b = DESP(i, 1) ^ DESP(i, 3) ^ DESP(i, 6) ^ 0x05;
		/*esto anterior es equivalente a si = b xor b(i+2)mod8 xor b(i+5)mod8 xor b(i+7)mod8 xor Di*/

		/*se procede a obtener el inverso*/
		sbox_i[i] = inverso(b);
	}
	/*se considera que inverso de '00' es '00' por lo tanto s[0]=C */
	sbox_i[0] = 0x05;
	return;
}

int main(int argc, char *argv[]){
	FILE *output = NULL; 
	int i;
	uint8_t sbox_d[256];
	uint8_t sbox_i[256];
    
	if (argc < 2 || argc > 4){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("/.SBOX_AES{-C | -D}[-o fileout]\n");
		printf("-C calcular la S-box directa\n");
		printf("-D calcular la S-box inversa\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {

		if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
			sbox_directa(sbox_d);
			if (argc > 2 && strcmp(argv[2], "-o") == 0) {
			 	output = fopen(argv[3], "w");
				if (output==NULL) {
					fputs ("File error",stderr); 
					exit (1);
				}
        	 	for (i=0;i<256;i++){
	 		 		fprintf(output, "%x\t",sbox_d[i]); 
	 		 	}
             	fclose(output);
			}else{
				for (i=0;i<256;i++){
					printf("%x\t",sbox_d[i]);
				}	
			}
		}else {
			sbox_inversa(sbox_i);
			if (argc > 2 && strcmp(argv[2], "-o") == 0) {
			 	output = fopen(argv[3], "w");
				if (output==NULL) {
					fputs ("File error",stderr); 
					exit (1);
				}
        	 	for (i=0;i<256;i++){
	 		 		fprintf(output, "%x\t",sbox_i[i]); 
	 		 	}
             	fclose(output);
			}else{
				for (i=0;i<256;i++){
					printf("%x\t",sbox_i[i]);
				}	
			}
		}
	}
	return 0;
}




