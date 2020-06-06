#include <stdio.h>
#include <stdlib.h>
#include "type.h"

int main(int argc, char const *argv[]) {

  FILE *f;
  long int val=9112317239;
  double val_dbl=9.3333331123172;
  int val_int=456778;
  char val_str [50] = "hola, soy una cadena";

  f = fopen ("prueba_type", "w");

  print_value(f, LLNG, &val);
  printf("\nMiralo que bonito: %ld\n", val);

  print_value(f, DBL, &val_dbl);
  printf("\nMiralo que bonito: %f\n", val_dbl);

  print_value(f, INT, &val_int);
  printf("\nMiralo que bonito: %d\n", val_int);

  print_value(f, STR, &val_str);
  printf("\nMiralo que bonito: %s\n", val_str);

  fclose(f);

  return 1;

}
