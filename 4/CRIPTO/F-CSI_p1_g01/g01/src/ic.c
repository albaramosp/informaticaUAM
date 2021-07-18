/**
 * @brief Programa que realiza el test del Índice de Coincidencia sobre un texto para obtener el tamaño de clave.
 * El programa se llama con la siguiente estructura:
 * IC [-l longitud cadenas ] [-i file in ] [-o file out ] 
 * -l longitud deseada para los Ngramas. 
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -o fichero de salida. Si no se proporciona, se utiliza la salida estandar
 * 
 * 
 * @file IC.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 13-10-2020
 * @copyright GNU Public License
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "euclides.h"
#include <float.h> /*para DBL_MAX*/


long longitudTexto;
int tam_clave_max=15;/*tamaño máximo de letras de la clave a buscar. Añadir 1 más pues t empieza en 1*/
int c;


/*-----------------------------------------------------------------------------
Nombre: ic

Descripción: Obtiene el tamaño de la clave con la que se ha cifrado el texto pasado por 
	     parámetros por el método del Índice de coincidencia.

Argumentos de entrada: 
1. text, texto cifrado.
2. longitudTexto, la longitud del texto del que se quiere obtener la clave

Retorno: 
- El tamaño de la clave de cifrado
-----------------------------------------------------------------------------*/
int ic(char* text, long longitudTexto);


int main(int argc, char *argv[]){
	FILE *fin;
	/*FILE *fout;*/
	char *text=NULL;
		  

	if(argc > 7) {
		printf("\nERROR: utilizar con los siguientes argumentos:\n");
		printf("IC [-i file_in] [-o file_out]>  \n\n");
	}

	if (argc > 1 && strcmp(argv[1], "-i") == 0){
		fin = fopen(argv[2], "r");
		if(fin == NULL) {
			printf("ERROR en apertura de fichero");
	      		return(1);
	   	  }
		 /* Obtener longitud del texto mediante puntero */
		fseek(fin, 0, SEEK_END);/*activa el indicador de posición de ficheros */
		longitudTexto = ftell(fin);/*Retorna la posición del puntero de lectura/escritura de un archivo*/
		rewind(fin); /* Recoloca el puntero */

		/*se reserva memoria para la longitud del texto de entrada*/
		text = malloc((longitudTexto+1) * sizeof(char));
		fread(text, 1, longitudTexto, fin); 

		fclose(fin);

	} else {
		/*entrada por pantalla se reserva memoria para 2048 caracteres*/
		text = malloc(2048 * sizeof(char));
	    	printf("\nIntroducir el mensaje a criptoanalizar: ");
	    	fgets (text, 2048, stdin);
	    	text[strlen(text)-1] = '\0';
		longitudTexto = strlen(text)+1;
	}


	printf("+++++  Indice de Coincidencia  +++++\n");
	c=ic(text, longitudTexto);
	printf("Tamaño clave=%d\n",c );

/*	if (argc > 1 && (strcmp(argv[1], "-o")) == 0){
		fout = fopen(argv[2], "w");
		if(fout == NULL) {
	   		printf("ERROR en apertura de fichero");
	      		return(1);
		}
		fputc(c,fout);
		fclose(fout);
	}
	if (argc > 1 && (strcmp(argv[3], "-o")) == 0){
		fout = fopen(argv[4], "w");
		if(fout == NULL) {
	   		printf("ERROR en apertura de fichero");
	      		return(1);
		}
		fputc(c,fout);
		fclose(fout);
	}
*/

	/*liberamos memoria */
	free (text);
	text=NULL;
	return 0;
}

int ic(char* text, long longitudTexto){

	int filas = 26;
	int **coincidencias;	

	int i;	 /* Recorre filas */
	int j;	 /* Recorre columnas*/	
	
	int tam_clave=0, t=1,k=0;
	char* str =NULL;
	char *aux = text, *aux2 = text, car; /* Para poner el texto con solo simbolos alfabeticos */
	float ic_esp=0.07328, ic=0.0,*frecuencias=NULL, dif=0;
	float frec_rel=0;
	float margen=DBL_MAX; /*se inicializa a la mayor distancia de ic_esp*/
	double porcentaje;

  	/* Lo ponemos todo en mayusculas */
  	for (i=0; i < longitudTexto; i++) {
    		text[i] = toupper((unsigned char) text[i]);
  	}

  	/* Dejamos el texto unicamente con caracteres alfabeticos */
  	for (i = 0; i < longitudTexto; i++){
    		car = *aux++;
    		if(car >= 'A' && car <= 'Z') {*aux2++ = car;}
  	}
  	
	longitudTexto = aux2 - text;
  
	/*guardamos text en str para iterar sobre él*/
	str=text;


	/*copiamos el texto en str para no perderlo en cada iteración de C1, C2...tamaño máximo de la clave*/
	for(t=1; t < tam_clave_max; t++, str=text){/*se repite tantas veces como el tamaño de la clave máxima indicada y se va avanzando por 
						     toda la cadena saltando el número de letras de la clave*/


		/*Reserva de Memoria para las filas*/
		coincidencias = (int **)malloc(filas*sizeof(int*));
		

		/*reserva de memoria para las columnas*/		
		for (i=0;i<filas;i++) {
			coincidencias[i] = (int*)malloc(t*sizeof(int)); 
			memset(coincidencias[i], 0, sizeof(int)*t);
		}

		/*se aumenta la memoria según aumentan las columnas para los sucesivos subcriptogramas (C1, C2...)*/
		frecuencias=(float*)realloc(frecuencias, t*sizeof(float)); 
		memset(frecuencias, 0, sizeof(float)*t);


		/* Damos Valores a la Matriz; i tiene el tamaño de cada subcriptograma*/
		for(i=0;i<longitudTexto/t;i++,str+=t){
			for(j=0;j<t;j++){
				coincidencias [str[j]-65][j]+=1;
			}
		}

	
		/*cálculo de la frecuencia relativa*/
		for(k=0;k<filas;k++){
			for(j=0;j<t;j++){
				porcentaje =(coincidencias[k][j]/((float)longitudTexto/t))*100;
				frec_rel=(porcentaje/100)*((porcentaje-1)/100);
				frecuencias[j]= frecuencias[j]+frec_rel;
			}
			for(j=0;j<t;j++){
				free(coincidencias[k]);
				coincidencias[k]=NULL;
			}
		}
		
		/*se suman los ic's de las t columnas*/
		for(j=0, ic=0.0;j<t;j++){
			ic+=frecuencias[j];
			
		}

		/*frecuencia media*/
		ic=ic/t;

		dif=ic-ic_esp;/*diferencia entre el ic calculado y el ic medio español*/
		
		if (dif<0){ 
			dif = dif*(-1);
		}


		/*se va actualizando "margen" con la menor diferencia*/
		if(margen>dif){ 
			margen = dif;
			tam_clave= t;

		}
		
		
		/*liberamos memoria*/
		for (j=0; j < t; j++){
			free(coincidencias[j]);
			coincidencias[j]=NULL;
		}
		free(coincidencias);
		coincidencias= NULL;

		
	}
	/*liberación de memoria*/
	free(frecuencias);
	frecuencias=NULL;
	
	return tam_clave;
}
