/**
 * @brief Programa que cifra y descifra numeros utilizando el metodo de cifrado afin.
 * El programa se llama con la siguiente estructura:
 * afin {-C|-D} {-m |Z m |} {-a N × } {-b N + } [-i f ile in ] [-o f ile out ] 
 * -C el programa cifra
 * -D el programa descifra
 * -m tamaño del espacio de texto cifrado
 * -a coeficiente multiplicativo de la función afı́n
 * -b término constante de la función afı́n
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file afin.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 01-10-2020
 * @copyright GNU Public License
 */

# include <stdio.h>
# include <stdlib.h>
# include <gmp.h>
# include <string.h>
# include <ctype.h>
# include "euclides.h"

#define MAX_TXT 2048

int textlen;

char * cifrar (mpz_t a, mpz_t b, mpz_t m, char *p){
	int i;
	char *c, *aux, *ascii;
	mpz_t x, y, t;

	c = malloc(MAX_TXT * sizeof(char));
	ascii = malloc(MAX_TXT * sizeof(char));
	aux = malloc(MAX_TXT * sizeof(char));

	mpz_init(x);
	mpz_init(y);
	mpz_init(t);

	mpz_set_str(t, "65", 10);

	for(i = 0; i < textlen; i++){
		if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
			c[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
		} else {
			sprintf(aux, "%d", toupper(p[i])-65); /* A=0, B=1, ... Z=25 */
			mpz_set_str(x, aux, 10);

			/* y = ax + b mod m */
			mpz_mul(y, a, x);
			mpz_add(y, y, b);
			mpz_fdiv_r(y, y, m);

			mpz_add(y, y, t); /* Valor del caracter cifrado en ASCII de A a Z, asegura que sea mostrable*/
			
			mpz_get_str(ascii, 10, y);
			
			c[i] = atoi(ascii);

		}					
	}

	c[i] = '\0';

	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(t);
	free(ascii);
	free(aux);

	return c;
}

char * descifrar (mpz_t a, mpz_t b, mpz_t m, mpz_t inv_a, char *q){
	int i;
	char *d, *aux, *ascii;
	mpz_t x, y, t, zero;

	d = malloc(MAX_TXT * sizeof(char));
	ascii = malloc(MAX_TXT * sizeof(char));
	aux = malloc(MAX_TXT * sizeof(char));

	mpz_init(x);
	mpz_init(y);
	mpz_init(t);
	mpz_init(zero);

	mpz_set_str(t, "65", 10);
	mpz_set_str(zero, "0", 10);

	for(i = 0; i < textlen; i++){
		if (toupper(q[i]) < 65 || toupper(q[i]) > 90){
			d[i] = q[i]; /* Si no es una letra, lo dejamos como está */ 
		} else {
			sprintf(aux, "%d", toupper(q[i])-65); /* A=0, B=1, ... Z=25 */
			mpz_set_str(y, aux, 10);

			/* x = (y - b) * a^-1 (mod m) */
			mpz_sub(y, y, b);
			mpz_mul(y, y, inv_a);

			mpz_fdiv_r(x, y, m);/*aritmética modular*/
			
			if (mpz_cmp(x, zero) < 0){/*si y < 0*/
				mpz_add(x, x, m);/*se suma m para hacerlo positivo*/
			}

			mpz_add(x, x, t); /* Valor del caracter descifrado en ASCII de A a Z, asegura que sea mostrable*/

			mpz_get_str(ascii, 10, x);
			
			d[i] = atoi(ascii);

		}					
	}

	d[i] = '\0';

	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(t);
	mpz_clear(zero);
	free(ascii);
	free(aux);

	return d;
}

int main( int argc, char *argv[]){
	FILE *input, *output;
	char *c, *d, *p, *q, *aux;
	mpz_t  a, b, m;
	mpz_t  mcd, x, y;

	if (argc < 8 || argc > 12){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("afin {-C|-D} {-m |Z m |} {-a N × } {-b N + } [-i f ile in ] [-o f ile out ]\n");
		printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");

		printf("-m tamaño del espacio de texto cifrado\n");

		printf("-a coeficiente multiplicativo de la función afı́n\n");
		printf("-b término constante de la función afı́n\n");

		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
		mpz_init(a);
		mpz_init(b);
		mpz_init(m);
		mpz_init(mcd);
		mpz_init(x);
		mpz_init(y);
		
		aux = malloc(MAX_TXT * sizeof(char));

		mpz_set_str(m, argv[3], 10);
		mpz_set_str(a, argv[5], 10);
		mpz_set_str(b, argv[7], 10);
		
		/* La funcion de cifrado y descifrado es inyectiva si a y b son coprimos */
		if (coprimos(a, m) == 1){
			if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
				/*entrada por fichero*/				
				if(argc > 8 && strcmp("-i", argv[8]) == 0){/**/
					input = fopen(argv[9], "r");
					if (input==NULL){
						printf("Error en apertura de fichero");
						exit(1);					
					}

					fseek(input, 0, SEEK_END);
					textlen = ftell(input);
					rewind(input);

					p = malloc((textlen+1) * sizeof(char));
					fread(p, 1, textlen, input);
					fclose(input);
				}
				/* Si no se proporciona el parámetro -i, se procesará la entrada estándar */
				else {
					printf("\nIntroducir el mensaje a cifrar: ");
					scanf ("%m[^\n]%*c", &p);
                    textlen = strlen(p);
				}

				c = cifrar(a, b, m, p);

				
				/***SALIDA DE TEXTO***/
				/* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
				if (argc == 8){
					printf("MENSAJE CIFRADO: %s", c);
				
				/*entrada por pantalla y salida por fichero*/
				} else if (argc < 11 && strcmp(argv[8], "-o") == 0) {
					output = fopen(argv[9], "w");
					fputs(c, output);
					fclose(output);

				/*entrada por fichero y salida por fichero*/
				} else if (argc < 11 && strcmp(argv[8], "-o") != 0){
					printf("MENSAJE CIFRADO: %s", c);
				} else {
					output = fopen(argv[11], "w");
					fputs(c, output);
					fclose(output);
				}
				printf("\n");
				free(c);
				free(p);
				

			} else if (strcmp(argv[1], "-D") == 0 || strcmp(argv[1], "-d") == 0){
					
					/***ENTRADA DE TEXTO A DESCIFRAR***/
					/*entrada por fichero*/				
					if(argc > 8 && strcmp("-i", argv[8]) == 0){
						input = fopen(argv[9], "r");
						if (input==NULL){
							printf("Error en apertura de fichero");
							exit(1);					
						}

						fseek(input, 0, SEEK_END);
						textlen = ftell(input);
						rewind(input);

						q = malloc((textlen+1) * sizeof(char));
						fread(q, 1, textlen, input);
						fclose(input);
					}

					/* Si no se proporciona el parámetro -i, se procesará la entrada estándar */
					/*entrada por pantalla*/
					else {
						printf("\nIntroducir el mensaje a descifrar: ");
						scanf ("%m[^\n]%*c", &q);
						textlen = strlen(q);
					}

					/*Obtenemos el inverso multiplicativo*/
					obtener_inverso (a, m, y);

					d = descifrar(a, b, m, y, q);

					/***SALIDA DE TEXTO***/
					/* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
					if (argc == 8){
						printf("MENSAJE DESCIFRADO: %s", d);

					/*entrada por pantalla y salida por fichero*/
					} else if (argc < 11 && strcmp(argv[8], "-o") == 0) {
						output = fopen(argv[9], "w");
						fputs(d, output);
						fclose(output);
					
					/*entrada por fichero y salida por fichero*/
					} else if (argc < 11 && strcmp(argv[8], "-o") != 0){
						printf("MENSAJE DESCIFRADO: %s", d);
					} else {
						output = fopen(argv[11], "w");
						fputs(d, output);
						fclose(output);
					}

					printf("\n");
					free(d);
					free(q);

			} else {
				printf("\nERROR: opcion desconocida\n");
			}
		}  else {
			printf("\nERROR: los parametros a y b no determinan una funcion inyectiva\n");
		}

		mpz_clear(a);
		mpz_clear(b);
		mpz_clear(m);
		mpz_clear(mcd);
		mpz_clear(x);
		mpz_clear(y);

		free(aux);
	}
	
	return 0;
}
