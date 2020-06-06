/*
 * Fichero que prueba el funcionamiento de la tabla de símbolos, formada
 * mediante dos tablas hash que siguen el formato especificado en hash.c.
 * El programa lee de un fichero de entrada las tareas a realizar en la
 * tabla de símbolos, y devuelve en otro fichero el resultado de las
 * mismas según la implementación solicitada.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tablaSimbolos.h"

#define MAX_LINE 30

int main(int argc, char const *argv[]) {
  TABLA_SIMBOLOS *t = crearTablaSimbolos();
  FILE *entrada;
  FILE *salida;
  char line[MAX_LINE];
  int more = 0;
  int i;

  if (argc != 3){
    printf("\nError: proporcionar ficheros de entrada y salida como argumentos\n");
    return 0;
  }

  entrada = fopen(argv[1], "r");
  salida = fopen(argv[2], "w");

  while( fgets(line, MAX_LINE, entrada) ){
    char id[MAX_LINE];
    char n[MAX_LINE];
    int opt = -1;
    int cont = 0;

    more = 0;

    for (i = 0; i < MAX_LINE; i++){
      if (more == 1){
        if (line[i] == '\r'){
          n[cont] = '\0';
          break;
        } else {
          n[cont] = line[i];
          cont++;
        }
      } else {
        if (line[i] == '\t' ){
          id[i] = '\0';
          more = 1;

        } else if (line[i] == '\r'){
          id[i] = '\0';
          printf("\ni = %d", i);
          printf("\nPalabra: %s, id[i] = %c, id[i-1] = %c, id[i-2] = %c", id, id[i], id[i-1], id[i-2]);

          break;
        } else {
          id[i] = line[i];
        }
      }
    }

    if (more != 0){
      opt = atoi(n);

      /*Entero positivo: inserción*/
      if (opt >= 0){
        if (insercion(t, id, VARIABLE, ESCALAR, opt, 1, -1, -1) == ERROR){
          fprintf(salida, "-1\t%s\n", id);
        } else {
          fprintf(salida, "%s\n", id);
        }

      /*-999: cerrar ámbito*/
    } else if (opt == -999) {
      if (cierreAmbitoLocal(t) == ERROR){
        fprintf(salida, "-1\t%s\n", id);
      } else {
        fprintf(salida, "%s\n", id);
      }

      /*Entero negativo: abrir ámbito*/
      } else {
        if (aperturaAmbitoLocal(t, id, opt, -1, -1) == ERROR){
          fprintf(salida, "-1\t%s\n", id);
        } else {
          fprintf(salida, "%s\n", id);
        }

      }

    /*Sin entero: búsqueda*/
    } else {
      int ret = busqueda(t, id);
      if (ret == -1){
        fprintf(salida, "%s\t-1\n", id);
      } else {
        fprintf(salida, "%s\t%d\n", id, ret);
      }
    }
  }

  fclose(entrada);
  fclose(salida);
  borrarTablaSimbolos(t);
  printf("\n");

  return 0;
}
