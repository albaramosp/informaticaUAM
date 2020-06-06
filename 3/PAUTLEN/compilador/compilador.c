/*
 * Compilador del lenguaje ALFA.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include "alfa.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>

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
    printf("\nError en el compilador\n");
  } else {
      printf("\nCompilacion correcta\n");
  }

  fclose(yyout);
  fclose(yyin);

  return 0;
}
