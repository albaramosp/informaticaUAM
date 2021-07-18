/**
 * @brief Programa que realiza el test de Kasiski sobre un texto para obtener el tamaño de clave.
 * El programa se llama con la siguiente estructura:
 * kasiski [-i file in ] [-l longitud cadenas ] 
 * -i fichero de entrada. Si no se proporciona, se utiliza la entrada estandar
 * -l longitud deseada para las subcadenas. Si no se proporciona, se utiliza 3 por defecto.
 * 
 * 
 * @file kasiski.c
 * @author Grupo 01 - CRIPTO
 * @version 1.0
 * @date 11-10-2020
 * @copyright GNU Public License
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "euclides.h"

char *text;
long longitudTexto;
long longitudSubcadena = 3;

char cadenas[100][2048]; /* Subcadenas encontradas */
int distancias[100][100]; /* Distancias entre las cadenas encontradas */
int contadores[100]; /* Indice para saber cuantas distancias encontradas tiene cada subcadena en total*/ 
int contCadenas = 0; /* Total subcadenas encontradas */

/*-----------------------------------------------------------------------------
Nombre: buscarSubcadenas

Descripción: Busca cadenas de tamaño longitudSubcadena repetidas en un texto. Las cadenas
las guarda en cadenas, y las distancias entre ellas en distancias. Asimismo, cada cadena encontrada
lleva asociado un contador de las veces que ha aparecido. 

Retorno: 
- 1 si se han encontrado cadenas repetidas del tamaño especificado, 0 si no.
-----------------------------------------------------------------------------*/
int buscarSubcadenas();

/*-----------------------------------------------------------------------------
Nombre: obtenerTamanoClave

Descripción: Obtiene el maximo comun divisor de las distancias que separan a la cadena
especificada en el indice del argumento. Si este mcd es 1, entonces no es valido y deberia
probarse con otra cadena.

Argumentos de entrada: 
1. index, indice de la cadena sobre la que queremos trabajar.

Retorno: 
- Tamaño de clave segun el test de kasiski
-----------------------------------------------------------------------------*/
int obtenerTamanoClave(int index);


int main(int argc, char *argv[]){
  FILE *f;
	int i, j;
  int n;
  int max; /* Veces que se repite la cadena mas veces encontrada */
  int indexMax; /* Indice de la cadena mas veces encontrada */

  if(argc > 5) {
 	  printf("\nERROR: utilizar con los siguientes argumentos:\n");
    printf("kasiski [-i file_in] [-l longitud_subcadena]>\n\n");
  }

  if (argc > 3){
      longitudSubcadena = atoi(argv[4]);
      if(longitudSubcadena < 1) {
        printf("ERROR: longitud de subcadena especificada menor que 1. Debe ser mayor o igual.");
        return (1);
      }
  }

  if (argc == 3 && strcmp(argv[1], "-l") == 0){
      longitudSubcadena = atoi(argv[2]);
      if(longitudSubcadena < 1) {
        printf("ERROR: longitud de subcadena especificada menor que 1. Debe ser mayor o igual.");
        return (1);
      }
  }

  if (argc > 1 && strcmp(argv[1], "-i") == 0){
	  f = fopen(argv[2], "r");
	  if(f == NULL) {
      printf("ERROR en apertura de fichero");
      return(1);
    }

	  /* Obtener longitud del texto mediante puntero */
	  fseek(f, 0, SEEK_END);
	  longitudTexto = ftell(f);
	  rewind(f); /* Recoloca el puntero */

	  text = malloc((longitudTexto+1) * sizeof(char));
	  fread(text, 1, longitudTexto, f); 
	  fclose(f);

  } else {
    printf("\nIntroducir el mensaje a criptoanalizar: ");
    scanf ("%m[^\n]%*c", &text);
    longitudTexto = strlen(text);
  }

  printf("\n---->Buscando cadenas de tamaño %ld en el texto...\n\n", longitudSubcadena);

  if (buscarSubcadenas() == 1){
    j = 0;
    while(j < contCadenas){
      max = 0;
      indexMax = 0;

      printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      for (i = 0; i < contCadenas; i++){
        /*printf("%s\tVeces encontrada: %d\n", cadenas[i], contadores[i]);*/
        if (contadores[i] > max){
          max = contadores[i];
          indexMax = i;
        }
      }

      printf("\nLa cadena que mas se repite es %s (%d veces)\n\n", cadenas[indexMax], max);
      n = obtenerTamanoClave(indexMax);

      if (n == 1){
        printf("\nDistancias no multiplos de n, volvemos a probar con otra cadena\n");
        contadores[indexMax] = 0;
        j++;
      } else {
        printf("\n\n----->n = %d segun Kasiski\n\n", n);
        break;
      }

    }
  } else {
    printf("\nVaya, no se han encontrado cadenas repetidas con la longitud especificada.\n");
  }

  free(text);

  return(0);
}

int obtenerTamanoClave(int index){
  int n = distancias[index][0];
  int i;

  printf("----->n|mcd(dist_1, dist_2, ... , dist_n)\n\n");

  for (i = 1; i < contadores[index]; i++){ 
      printf("mcd(%d, %d) = ", n, distancias[index][i]);
      n = mcd(distancias[index][i], n); 
      printf("%d\n", n);

      if(n == 1){ 
          return 1; 
      } 
  } 

  return n; 
}

int buscarSubcadenas(){
  long i, j, i0 = -1, k0 = 0;
  char *aux, *aux2, c; /* Para poner el texto con solo simbolos alfabeticos */
  char t[2048];
  int flag = 0; /* Para ver si una subcadena ya estaba registrada */
  int found = 0; /* Para saber si hay al menos una subcadena */

  aux = text;
  aux2 = text;
  
  /* Lo ponemos todo en mayusculas */
  for (i=0; i < longitudTexto; i++) {
    text[i] = toupper((unsigned char) text[i]);
  }
  
  /* Dejamos el texto unicamente con caracteres alfabeticos */
  for (i = 0; i < longitudTexto; i++){
    c = *aux++;
    if (c >= 'A' && c <= 'Z') {
      *aux2++ = c;
    }
  }

  longitudTexto = aux2 - text;
  
  for (i = 0; i < longitudTexto; i++){ /* Para cada caracter del texto... */
    for (j = i+1; j < longitudTexto; j++){ /* ... Comparamos con el resto de caracteres */
      if (text[i] == text[j]){ /* Coincidencia de dos caracteres */
        long k, l;
        /* Aumentamos k cada vez que haya un caracter mas de la subcadena coincidente */
        for (k = 0; text[i+k] == text[j+k]; k++){
          if (k == longitudSubcadena+1){ /* Paramos cuando lleguemos a la longitud deseada */
            break;
          }
        }

        /* La subcadena encontrada tiene la longitud minima requerida */   
        /* Comparamos indices para determinar que no estamos ante un caso 'aaaaaa' por ejemplo, que estamos ante dos subcadenas distintas */     
        if (k == longitudSubcadena && (i > i0+k0 || k-k0 > i-i0)){
          i0 = i; k0 = k;
          flag=0;
          found=1;

          for (l = 0; l < k; l++){
            t[l] = text[i+l];
          }

          t[l] = '\0';

          /* Comprobamos si ya teníamos esta cadena previamente registrada */
          for (l = 0; l < contCadenas; l++){
            if (strcmp(t, cadenas[l])==0){
              flag = 1;
              break;
            }
          }

          if (flag == 1){ /* La subcadena ya la teniamos */
            distancias[l][contadores[l]] = (j+1)-(i+1);
            contadores[l]++;
            /*printf("Otra vez %s, ya van %d y esta dist es %d\n", cadenas[l], contadores[l], distancias[l][contadores[l]-1]);
            */

          } else { /* Nueva subcadena */
            strcpy(cadenas[contCadenas], t);
            contadores[contCadenas] = 1;
            distancias[contCadenas][contadores[contCadenas]-1] = (j+1)-(i+1);
            /*printf("New: %s, distance %d\n", cadenas[contCadenas], distancias[contCadenas][contadores[contCadenas]-1]);
            */
            contCadenas++;
          }
        }
      }
    }
  }

  /* Tenemos guardadas subcadenas, distancias y veces que aparecen */
  return found;
}
