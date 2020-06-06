/**************************************************/
/* Programa: ejercicio4       Fecha: 28/09/2018   */
/* Autores:alba Ramos, Javier Lozano              */
/*                                                */
/* Programa que Comprueba SelectSort              */
/*                                                */
/* Entrada: Linea de comandos                     */
/* -tamanio: numero elementos permutacion         */
/* Salida: 0: OK, -1: ERR                         */
/**************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "permutaciones.h"
#include "ordenacion.h"

int main(int argc, char** argv)
{
  int tamano, i, j, ret;
  int* perm = NULL;

  srand(time(NULL));

  if (argc != 3) {
    fprintf(stderr, "Error en los parametros de entrada:\n\n");
    fprintf(stderr, "%s -tamanio <int>\n", argv[0]);
    fprintf(stderr, "Donde:\n");
    fprintf(stderr, " -tamanio : numero elementos permutacion.\n");
    return 0;
  }
  printf("Practica numero 1, apartado 4\n");
  printf("Realizada por: alba Ramos y Javier Lozano\n");
  printf("Grupo: 1212\n");

  /* comprueba la linea de comandos */
  for(i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-tamanio") == 0) {
      tamano = atoi(argv[++i]);
    } else {
      fprintf(stderr, "Parametro %s es incorrecto\n", argv[i]);
    }
  }

  perm = genera_perm(tamano);

  if (perm == NULL) { /* error */
    printf("Error: No hay memoria\n");
    exit(-1);
  }

  printf("Tabla original:\n");
  for(j = 0; j < tamano; j++) {
    printf("%d \t", perm[j]);
  }
  printf("\n");

  ret = SelectSort(perm, 0, tamano-1);

  if (ret == ERR) {
    printf("Error: Error en BubbleSort\n");
    free(perm);
    exit(-1);
  }

  printf("Tabla ordenada:\n");
  for(j = 0; j < tamano; j++) {
    printf("%d \t", perm[j]);
  }
  printf("\n");

  ret = SelectSortInv(perm, 0, tamano-1);

  if (ret == ERR) {
    printf("Error: Error en BubbleSort\n");
    free(perm);
    exit(-1);
  }

  printf("Tabla ordenada inv:\n");
  for(j = 0; j < tamano; j++) {
    printf("%d \t", perm[j]);
  }
  printf("\n");

  free(perm);

  return 0;
}
