/**
 * @brief Programa que cifra y descifra realizando una doble permutacion sobre el mensaje colocado
 * en forma de matriz de tamaño mxn, donde m es la longitud de k1 y n la de k2. La permutacion de filas viene
 * dada por k1 y la de columnas por k2, de forma que la fila 1 de la matriz resultante es k1[1], la segunda es k1[2], etc.
 * Y la columna 1 de la matriz resultante es k1[1], la segunda es k2[2], y así sucesivamente.
 * El programa se llama con la siguiente estructura:
 * permutacion {-C|-D} {-k1 k1 -k2 k2} [-i f ile in ] [-o f ile out ] 
 * -C el programa cifra
 * -D el programa descifra
 * -k1, k2 cadenas con las claves
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file permutacion.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 22-10-2020
 * @copyright GNU Public License
 */

# include <stdio.h>
# include <stdlib.h>
# include <gmp.h>
# include <string.h>
# include <ctype.h>
# include "euclides.h"

#define MAX_TXT 2048
int m, n, textlen;
char **matriz;

void construirMatriz(char *texto, int indice){
    int i, j;

    for (i = 0; i < n; i++){
        for (j = 0; j < m; j++){
            if (indice < textlen){
                matriz[j][i] = texto[indice];
                indice++;
            } else {
                matriz[j][i] = '.';
            }
            
        }
    }

    /* Te la enseño */
    printf("\nConstruida matriz\n");
    for(i = 0; i < m; i++){
        for(j = 0; j < n; j++){
            printf("%c   ", matriz[i][j]);
        }

        printf("\n");
    }
    
}

void permutarMatriz(int *k1, int *k2){
    int i, j;
    char copia[m][n];

    for (i = 0; i < m; i++){
        for (j = 0; j < n; j++){
            copia[i][j] = matriz[i][j];
        }
    }

    /* Se permuta siguiendo el algoritmo especificado */
    for (i = 0; i < m; i++){
        for (j = 0; j < n; j++){
            matriz[i][j] = copia[k1[i]-1][k2[j]-1];
        }
    }

    printf("\nPermutada matriz: \n");
    /* Te la enseño */
    for(i = 0; i < m; i++){
        for(j = 0; j < n; j++){
            printf("%c   ", matriz[i][j]);
        }

        printf("\n");
    }
}


char * cifrar (char *p, int *k1, int *k2){
	int indice, i, j, cont;
	char *c;

	c = malloc(textlen * sizeof(char));

    indice = 0;
    cont = 0;

    while (indice < textlen){
        construirMatriz(p, indice);
        indice += m*n;
        permutarMatriz(k1, k2);

        for (i = 0; i < n; i++){
            for (j = 0; j < m; j++){
                c[cont] = matriz[j][i];
                cont++;
            }
        }
    }

	return c;
}

char * descifrar (char *c, int *k1, int *k2){
	int indice, i, j, cont, *aux, *aux2;
	char *p;

	p = malloc(textlen * sizeof(char));
    aux = malloc(m * sizeof(int));
    aux2 = malloc(n * sizeof(int));

    /* Copiamos las permutaciones para poder invertirlas */ 
    for (i = 0; i < m; i++){
        aux[i] = k1[i];
    }
    for (i = 0; i < n; i++){
        aux2[i] = k2[i];
    }

    /* Invertimos las permutaciones de k1 y k2 */
    for (i = 0; i < m; i++){
        k1[aux[i]-1] = i+1;
    }
    printf("\nModificado k1: ");
    for (i = 0; i < m; i++){
        printf("%d", k1[i]);
    }

    for (i = 0; i < n; i++){
        k2[aux2[i]-1] = i+1;    
    }
    printf("\nModificado k2: ");
    for (i = 0; i < n; i++){
        printf("%d", k2[i]);
    }

    indice = 0;
    cont = 0;

    while (indice < textlen){
        construirMatriz(c, indice);
        indice += m*n;
        permutarMatriz(k1, k2);

        for (i = 0; i < n; i++){
            for (j = 0; j < m; j++){
                p[cont] = matriz[j][i];
                cont++;
            }
        }
    }

    free(aux);
    free(aux2);

    return p;
}


int main( int argc, char *argv[]){
	FILE *input, *output;
	char *c, *p;
    int *k1, *k2;
    int i;

	if (argc < 6 || argc > 10){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("permutacion {-C|-D} {-k1 k1 -k2 k2} [-i f ile in ] [-o f ile out ] \n");

		printf("-C el programa cifra\n");
		printf("-D el programa descifra\n");

		printf("-k1, k2 cadenas con las claves\n");

		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
        m = strlen(argv[3]); /* Filas */
        n = strlen(argv[5]); /* Columnas */
        k1 = malloc(m * sizeof(int));
        k2 = malloc(n * sizeof(int));
        printf("Matriz de %dx%d\n", m, n);

        for (i = 0; i < m; i++){
            k1[i] = (argv[3][i])-'0';
            printf("%d", k1[i]);
        }
        printf("\n");

        for (i = 0; i < n; i++){
            k2[i] = (argv[5][i])-'0';
            printf("%d", k2[i]);
        }

        matriz = malloc(m * sizeof(char *));
        for (i = 0; i < m; i++){
            matriz[i] = malloc(n * sizeof(char));
        }
		
		
        if (strcmp(argv[1], "-C") == 0 || strcmp(argv[1], "-c") == 0){
            if(argc > 6 && strcmp("-i", argv[6]) == 0){
				input = fopen(argv[7], "r");
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

            c = cifrar(p, k1, k2);

            
            /* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
            if (argc == 6){
                printf("MENSAJE CIFRADO: %s", c);
            } else if (argc > 6 && strcmp(argv[6], "-o") == 0) {
                output = fopen(argv[7], "w");
                fputs(c, output);
                fclose(output);
            } else if (argc == 10){
                output = fopen(argv[9], "w");
                fputs(c, output);
                fclose(output);
            } else {
                printf("MENSAJE CIFRADO: %s", c);
            }

            printf("\n");
            free(c);
            free(p);

        } else if (strcmp(argv[1], "-D") == 0 || strcmp(argv[1], "-d") == 0){
            if(argc > 6 && strcmp("-i", argv[6]) == 0){
				input = fopen(argv[7], "r");
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

            p = descifrar(c, k1, k2);

            
            /* Si no se proporciona el parámetro -o, se utilizará la salida estándar */
            if (argc == 6){
                printf("MENSAJE DESCIFRADO: %s", p);
            } else if (argc > 6 && strcmp(argv[6], "-o") == 0) {
                output = fopen(argv[7], "w");
                fputs(p, output);
                fclose(output);
            } else if (argc == 10){
                output = fopen(argv[9], "w");
                fputs(p, output);
                fclose(output);
            } else {
                printf("MENSAJE DESCIFRADO: %s", p);
            }

            printf("\n");
            free(c);
            free(p);

        } else {
            printf("\nERROR: opcion desconocida\n");
        }

        for (i = 0; i < m; i++){
            free(matriz[i]);
        }

        free(matriz);
        free(k1);
        free(k2);
	}

    
	
	return 0;
}
