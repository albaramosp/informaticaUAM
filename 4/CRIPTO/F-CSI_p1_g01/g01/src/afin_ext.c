/**
 * @brief Programa que cifra y descifra utilizando cifrado afin y permutacion.
 * El programa se llama con la siguiente estructura:
 * afin_ext {-C|-D} {-m |Z m |} {-a N × } {-b N + } [-i f ile in ] [-o f ile out ] [-p p1 p2 ... pm] 
 * -C el programa cifra
 * -D el programa descifra
 * -m tamaño del espacio de texto cifrado
 * -a coeficiente multiplicativo de la función afı́n
 * -b término constante de la función afı́n
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * -p permutacion deseada, indica el orden de cada caracter. Por ejemplo: 4, 3, 2 indica que la A pase a 
 * ser el caracter cuarto, la B el tercero y la C el segundo, dejando el resto en el orden normal.
 * Si no se especifica, la permutacion por defecto es darle la vuelta al alfabeto.
 * 
 * 
 * @file afin_ext.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 25-09-2020
 * @copyright GNU Public License
 */

# include <stdio.h>
# include <stdlib.h>
# include <gmp.h>
# include <string.h>
# include <ctype.h>
# include "euclides.h"

# define MAX_TXT 100

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

	for(i = 0; i < strlen(q); i++){
		if (toupper(q[i]) < 65 || toupper(q[i]) > 90){
			d[i] = q[i]; /* Si no es una letra, lo dejamos como está */ 
		} else {
			sprintf(aux, "%d", toupper(q[i])-65); /* A=1, B=2, ... Z=26 */
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

void permutar (int argc, char **argv, char *p, char *p_perm, int *perm){
    int i, j;

    if (argc == 8){ /* Permutacion por defecto: darle la vuelta al alfabeto */
        for(i = 0; i < textlen; i++){
            if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
                p_perm[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
            } else {
                p_perm[i] = (27-(toupper(p[i])-64))+64;
            }				
        }
    } else if (argc > 9 && strcmp(argv[8], "-p") == 0) { /* Permutacion pasada por argumento */
        for(i = 9, j = 0; i < argc; i++, j++){
            perm[j] = atoi(argv[i]);			
        }

        /* El resto de elementos se quedan como estan */
        for (j = j; j < atoi(argv[3]); j++){
            perm[j] = j+1;
        }

        for(i = 0; i < textlen; i++){
            if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
                p_perm[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
            } else {
                p_perm[i] = perm[toupper(p[i])-65]+64; /* Indexamos cada elemento para asociarlo a su numero de permutacion y lo pasamos a ascii */
            }				
        }
    } else if (argc > 11 && strcmp(argv[10], "-p") == 0){
        for(i = 11, j = 0; i < argc; i++, j++){
            perm[j] = atoi(argv[i]);			
        }

        /* El resto de elementos se quedan como estan */
        for (j = j; j < atoi(argv[3]); j++){
            perm[j] = j+1;
        }

        for(i = 0; i < textlen; i++){
            if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
                p_perm[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
            } else {
                p_perm[i] = perm[toupper(p[i])-65]+64;
            }				
        }

    } else if (argc > 13 && strcmp(argv[12], "-p") == 0){
        for(i = 13, j = 0; i < argc; i++, j++){
            perm[j] = atoi(argv[i]);		
        }

        /* El resto de elementos se quedan como estan */
        for (j = j; j < atoi(argv[3]); j++){
            perm[j] = j+1;
        }

        for(i = 0; i < textlen; i++){
            if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
                p_perm[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
            } else {
                p_perm[i] = perm[toupper(p[i])-65]+64;
            }					
        }

    } else { /* Permutacion por defecto: darle la vuelta al alfabeto */
        for(i = 0; i < textlen; i++){
            if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
                p_perm[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
            } else {
                p_perm[i] = (27-(toupper(p[i])-64))+64;
            }	
        }
    }
}

int main( int argc, char *argv[]){
	FILE *input, *output;
	char *c, *p, *p_perm;
    int *perm;
	mpz_t  a, b, m, y;

    if (argc < 8){
        printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("afin_ext {-C|-D} {-m |Z m |} {-a N × } {-b N + } [-i f ile in ] [-o f ile out ] [-p p1 p2 ... pm]\n");
		printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");

		printf("-m tamaño del espacio de texto cifrado\n");

		printf("-a coeficiente multiplicativo de la función afı́n\n");
		printf("-b término constante de la función afı́n\n");

		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("-p permutacion de los elementos del alfabeto\n");
		printf("\n");
    } else {
        mpz_init(a);
		mpz_init(b);
		mpz_init(m);
		mpz_init(y);
		p_perm = malloc(MAX_TXT * sizeof(char));
        perm = malloc(atoi(argv[3]) * sizeof(int));

		mpz_set_str(m, argv[3], 10);
		mpz_set_str(a, argv[5], 10);
		mpz_set_str(b, argv[7], 10);
		
		/* La funcion de cifrado y descifrado es inyectiva si a y b son coprimos */
		if (coprimos(a, b) == 1){
            if (strcmp(argv[1], "-D") == 0 || strcmp(argv[1], "-d") == 0){
                if(argc > 8 && strcmp("-i", argv[8]) == 0){
					input = fopen(argv[9], "r");
                    if (input==NULL){
                        printf("Error en apertura de fichero");
                        exit(1);					
                    }

                    fseek(input, 0, SEEK_END);
                    textlen = ftell(input);
                    rewind(input);

                    c = malloc((textlen+1) * sizeof(char));
                    fread(c, 1, textlen, input);
                    fclose(input);
				}

				/* Si no se proporciona el parámetro -i, se procesará la entrada estándar */
				else {
					printf("\nIntroducir el mensaje a descifrar: ");
					scanf ("%m[^\n]%*c", &c);
                    textlen = strlen(c);
				}

                /* Aplicamos el descifrado afin sobre el texto */
				obtener_inverso (a, m, y);
                p = descifrar(a, b, m, y, c);


                /* Aplicamos la permutacion despues */
                strcpy(p_perm, "test"); /* Inicializacion del vector */
                permutar(argc, argv, p, p_perm, perm);

				/* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
				if (argc == 8){
					printf("MENSAJE DESCIFRADO: %s", p_perm);
				} else if (argc > 9 && strcmp(argv[8], "-o") == 0) {
					output = fopen(argv[9], "w");
					fputs(p_perm, output);
					fclose(output);
				} else if (argc > 11 && strcmp(argv[10], "-o") == 0){
                    output = fopen(argv[11], "w");
					fputs(p_perm, output);
					fclose(output);
				} else {
					printf("MENSAJE DESCIFRADO: %s", p_perm);
				}

				printf("\n");

            } else if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
                if(argc > 8 && strcmp("-i", argv[8]) == 0){
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

                /* Aplicamos la permutacion primero */
                strcpy(p_perm, "test"); /* Inicializacion del vector */
                permutar(argc, argv, p, p_perm, perm);
                printf("MENSAJE PERMUTADO: %s\n", p_perm);

                
                /* Aplicamos el cifrado afin sobre el texto permutado */
                c = cifrar(a, b, m, p_perm);

				/* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
				if (argc == 8){
					printf("MENSAJE CIFRADO: %s", c);
				} else if (argc > 9 && strcmp(argv[8], "-o") == 0) {
					output = fopen(argv[9], "w");
					fputs(c, output);
					fclose(output);
				} else if (argc > 11 && strcmp(argv[10], "-o") == 0){
                    output = fopen(argv[11], "w");
					fputs(c, output);
					fclose(output);
				} else {
					printf("MENSAJE CIFRADO: %s", c);
				}

				printf("\n");

            } else {
                printf("\nERROR: opcion desconocida\n");
            }
        } else {
			printf("\nERROR: los parametros a y b no determinan una funcion inyectiva\n");
		}
        
        mpz_clear(a);
		mpz_clear(b);
		mpz_clear(m);
        mpz_clear(y);
		free(c);
		free(p);
        free(p_perm);
        free(perm);
    }

    return 0;
}
