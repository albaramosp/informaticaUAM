/**
 * @brief Programa que comprueba que el método de cifrado por desplazamiento:
 *	1. Consigue Seguridad Perfecta si se eligen las claves con igual probabilidad.
 *	2. No consigue Seguridad Perfecta si las claves no son equiprobables. 
 * El programa se llama con la siguiente estructura:
 * ./seg-perf [-P | -I ] [-i file in ] [-o file out ] 
 * -P si quieres que sea equiprobable la distribucion de claves, -I si no quieres
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file seg-perf.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 29-10-2020
 * @copyright GNU Public License
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>


FILE *output = NULL;
int textlen;
long longitudTexto;
double p_x[26];
double p_y[26];
double p_xy[26][26];/*guarda las ocurrencias de cada par*/
double p_xy_f[26][26];/*guarda las frecuencias de cada par*/
char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void eliminarEspacios(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}

void comprobarSeguridadPerfecta(char *p, char *equiprobables){
    /*int diferentes = 0;*/ /* Flag que se activa si P(x|y) != P(x) en algun caso */
    char k; /* Clave generada cada vez */
    int i, j, cont;
    time_t t;
    double aux; /* P(x|y) */
    char *c; /* Texto cifrado */

	c = malloc((textlen+1)*sizeof(char));/*guarda el texto cifrado*/

	srand((unsigned) time(&t));

    /* Se inicializan los arrays con frecuencias */ 
	for (i = 0; i < 26; i++){
        	p_x[i] = 0;
		p_y[i] = 0;
    }

	for (i = 0; i < 26; i++){
        	for (j = 0; j < 26; j++){
            		p_xy[i][j] = 0;
			p_xy_f[i][j] = 0;
        }
    }

	/* Cifrado con claves distintas por cada caracter */
	for (i = 0; i < textlen; i++){
		k = alfabeto[rand() % 26]; /* Clave aleatoria */

		if(strcmp("-I",equiprobables)==0){
			/*1º sesgo: si la k es A, D, E u O, k=Z*/			
			if(k==65 || k== 68 || k== 69 || k == 79 ){
			/*if(k>=70 && k<=89){*/
				k=90; /* Sesgamos hacia la Z */
			}
		}
		c[i] = ( ( ( toupper(p[i])-65 ) + ( toupper(k)-65 ) ) % 26 ) + 65; /*guardamos en c el texto cifrado*/
	}

	c[i] = '\0';

	/* Calculamos las ocurrencias del texto plano:P_p(x) */
	for (i = 0; i < textlen; i++){
	        p_x[( toupper(p[i])-65 )]+=1;
	}

	/*Calculamos la frecuencia de cada letra del texto plano*/
	for (i = 0; i < 26; i++){
		p_x[i] = p_x[i] / textlen;
		
		if (output != NULL){/*escritura a fichero*/
			fprintf(output, "Pp(%c) = %lf\n", alfabeto[i], p_x[i]);
		} else {
			printf("Pp(%c)=%lf\n", alfabeto[i], p_x[i]);
		}
	}


	/* Calculamos las ocurrencias del texto cifrado:P_c(y) */
	for (i = 0; i < textlen; i++){
	        p_y[( toupper(c[i])-65 )]+=1;
	}

	/*Calculamos la frecuencia de cada letra del texto cifrado*/
	for (i = 0; i < 26; i++){
		p_y[i] = p_y[i] / textlen;
	}



	/* Calculamos las ocurrencias P(x,y) */
	for (i = 0; i < textlen; i++){
		p_xy[(toupper(p[i])-65)][(toupper (c[i])-65)]+=1;
	}

	/*Calculamos la frecuencia/probabilidad de cada par de letras*/
	for (i = 0; i < 26; i++){
		for (j = 0; j < 26; j++){
			p_xy_f[i][j] = p_xy[i][j] / textlen;
		}
	}
	/*calculamos p (x|y)=P(x,y)/P(y)*/
	for (i = 0; i < 26; i++){
		for (j = 0; j < 26; j++){
			/*se divide por p(y)*/
			p_xy_f[i][j] = p_xy_f[i][j]/p_y[j];
			
			if (output != NULL){/*escritura a fichero*/
				fprintf(output, "p(%c | %c) = %lf\t", alfabeto[i], alfabeto[j], p_xy_f[i][j]);
			} else {
				printf("p(%c | %c) = %lf\t", alfabeto[i], alfabeto[j], p_xy_f[i][j]);
			}
		}
	}

	/*Hacemos la media de las probabilidades de cada par de letras cuya letra del texto plano es igual: P[A][A]=P[A][B]=P[A][C]=...=P[A]*/	
	/*solo para verificar pero no se usa pues en la práctica no lo piden*/	
	for (i = 0; i < 26; i++){
		cont=0;
		aux=0;
		for (j = 0; j < 26; j++){
			if(p_xy_f[i][j]>0){/*si hay frecuencia para el par de letras*/
				aux = aux + p_xy_f[i][j];
				cont++;
			}
		}
		aux = aux/cont;
	/*	printf("Media de p (%c |---)= %lf y p[%c]= %lf\n", alfabeto[i],aux,alfabeto[i],p_x[i]);*/
	/*	printf("\n");										*/
	}
	
    free(c);
}


int main(int argc, char *argv[]){
    char *mensaje=NULL;
	FILE *input = NULL;
	int i;
	char *equiprobables;
	char *aux, *aux2, c; /* Para poner el texto con solo simbolos alfabeticos */
	    
	if (argc < 2 || argc > 6){
		printf("ERROR: utilizar con los siguientes argumentos:\n");
		printf("seg-perf {-P | -I} [-i f ile in ] [-o f ile out ]\n");
	
		printf("-P si quieres que sea equiprobable la distribucion de claves, -I si no quieres\n");
		printf("-i fichero de entrada\n");
		printf("-o fichero de salida\n");
		printf("\n");
	} else {
		equiprobables = argv[1];

	if (strcmp(argv[1], "-P" )!= 0 && strcmp(argv[1], "-p") !=0 &&  strcmp(argv[1], "-i") !=0 && strcmp("-I", argv[1])!=0){
                printf("ERROR: elige equiprobables= 'P' o no equiprobables= 'I'\n");
                return 1;
        }

        if (argc > 2 && strcmp("-i", argv[2]) == 0){
            input = fopen(argv[3], "r");
            if (input==NULL){
                printf("Error en apertura de fichero de entrada");
                exit(1);					
            }
    
            fseek(input, 0, SEEK_END);
            textlen = ftell(input);
            rewind(input);

	    	/*se reserva memoria para la longitud del texto de entrada*/
            mensaje = malloc((textlen+1) * sizeof(char));
            fread(mensaje, 1, textlen, input);
            fclose(input);
        }

        /* Si no se proporciona el parámetro -i, se procesará la entrada estándar */
        else {
	    printf("\nIntroducir el mensaje sobre el que obtener la clave: ");		
            scanf ("%m[^\n]%*c", &mensaje);
            textlen = strlen(mensaje);
        }

		mensaje[textlen] = '\0';

		aux = mensaje;
		aux2 = mensaje;
		
		/* Lo ponemos todo en mayusculas */
		for (i=0; i < textlen; i++) {
			mensaje[i] = toupper((unsigned char) mensaje[i]);
		}
		
		/* Dejamos el texto unicamente con caracteres alfabeticos */
		for (i = 0; i < textlen; i++){
			c = *aux++;
			if (c >= 'A' && c <= 'Z') {
			*aux2++ = c;
			}
		}

		textlen = aux2 - mensaje;
		mensaje[textlen] = '\0';
	

		/* Apertura de fichero de salida si es el caso */
		if (argc > 2 && strcmp(argv[2], "-o") == 0) {
			output = fopen(argv[3], "w");
			if (output==NULL){
                		printf("Error en apertura de fichero de salida");
                		exit(1);					
            		}
		} else if (argc == 6){
			output = fopen(argv[5], "w");
			if (output==NULL){
                		printf("Error en apertura de fichero de salida");
                		exit(1);					
            		}   
		}

		comprobarSeguridadPerfecta(mensaje, equiprobables);
		/*printf("\n");*/

		if (argc > 2 && strcmp(argv[2], "-o") == 0) {
			fclose(output);
		} else if (argc == 6){
			fclose(output);  
		}
		
		/*printf("\n");*/
		free(mensaje);
		mensaje=NULL;
    }

    return (0);
}
