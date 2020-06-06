/*
 * Programa que prueba el funcionamiento de alfa.l y alfa.y
 * Recibe un fichero de entrada en ALFA a procesar, y devuelve el resultado del analisis sintactico en un fichero de salida.
 * El programa procesa cada retorno recibido de alfa.y.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

int main (int argc, char **argv){
  extern FILE *yyin;
  extern FILE *yyout;

  if (argc < 3){
    printf("\nPor favor, introduce los ficheros de entrada y salida como argumentos.");
    return -1;
  }

  yyin = fopen(argv[1], "r");
  yyout = fopen(argv[2], "w");

  if(yyparse() != 0){
    printf("\nError en el analizador sintáctico\n");
  }

  fclose(yyout);
  fclose(yyin);

  return 0;
}
