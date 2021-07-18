/**
 * @brief Programa que cifra y descifra numeros utilizando el metodo de cifrado Vigenere.
 * El programa se llama con la siguiente estructura:
 * vigenere {-C|-D} {-k clave} [-i f ile in ] [-o f ile out ] 
 * -C el programa cifra
 * -D el programa descifra
 * -k cadena con la clave
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file vigenere.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 08-10-2020
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

char * cifrar (char *p, char *k, int n){
	int i, kCont;
	char *c;

	c = malloc((textlen+1) * sizeof(char));

	for(i = 0, kCont = 0; i < textlen; i++){
		if (toupper(p[i]) < 65 || toupper(p[i]) > 90){
			c[i] = p[i]; /* Si no es una letra, lo dejamos como está */ 
		} else {
            /* La clave se ira repitiendo cada n caracteres */
            if (kCont == n){
                kCont = 0;
            }

            /* xi + ki mod m */
            c[i] = ( ( ( toupper(p[i])-65 ) + ( toupper(k[kCont])-65 ) ) % 26 ) + 65; 
            kCont++;
		}					
	}

	c[i] = '\0';


	return c;
}

char * descifrar (char *c, char *k, int n){
	int i, kCont;
	char *p;

	p = malloc((textlen+1) * sizeof(char));

	for(i = 0, kCont = 0; i < textlen; i++){
		if (toupper(c[i]) < 65 || toupper(c[i]) > 90){
			p[i] = c[i]; /* Si no es una letra, lo dejamos como está */ 
		} else {
            /* La clave se ira repitiendo cada n caracteres */
            if (kCont == n){
                kCont = 0;
            }

            /* yi - ki mod m */
            /* yi + (m - ki) mod m */
            p[i] = ( ( ( toupper(c[i])-65 ) + (26 - (toupper(k[kCont])-65) ) ) % 26 ) + 65; 
            kCont++;
		}					
	}

	p[i] = '\0';

	return p;
}


int main( int argc, char *argv[]){
	FILE *input, *output;
	char *c, *p, *k, *aux;
    int n;

	if (argc < 4 || argc > 8){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("vigenere {-C|-D} {-k clave} [-i f ile in ] [-o f ile out ]\n");

		printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");

		printf("-k cadena con clave\n");

		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
		n = strlen(argv[3]);
        k = malloc((n+1) * sizeof(char));
		aux = malloc(MAX_TXT * sizeof(char));

        strcpy(k, argv[3]);
		
        if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
            if(argc > 4 && strcmp("-i", argv[4]) == 0){
				input = fopen(argv[5], "r");
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

            c = cifrar(p, k, n);
            
            /* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
            if (argc == 4){
                printf("MENSAJE CIFRADO: %s", c);
            } else if (argc > 4 && strcmp(argv[4], "-o") == 0) {
                output = fopen(argv[5], "w");
                fputs(c, output);
                fclose(output);
            } else if (argc == 8){
                output = fopen(argv[7], "w");
                fputs(c, output);
                fclose(output);
            } else {
                printf("MENSAJE CIFRADO: %s", c);
            }

            printf("\n");

        } else if (strcmp(argv[1], "-D") == 0 || strcmp(argv[1], "-d") == 0){
            if(argc > 4 && strcmp("-i", argv[4]) == 0){
				input = fopen(argv[5], "r");
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

            p = descifrar(c, k, n);
            
            /* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
            if (argc == 4){
                printf("MENSAJE DESCIFRADO: %s", p);
            } else if (argc > 4 && strcmp(argv[4], "-o") == 0) {
                output = fopen(argv[5], "a");
                fputs(p, output);
                fclose(output);
            } else if (argc == 8){
                output = fopen(argv[7], "a");
                fputs(p, output);
                fclose(output);
            } else {
                printf("MENSAJE DESCIFRADO: %s", p);
            }

            printf("\n");
        } else {
            printf("\nERROR: opcion desconocida\n");
        }

        free(p);
        free(c);
	    free(aux);
		free(k);
		
	}

	
	return 0;
}
