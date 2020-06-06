/*
 * Libreria que define lo necesario para manejar atributos en alfa.y.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#ifndef ALFA_H
#define ALFA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LONG_ID 100
#define MAX_TAMANIO_VECTOR 64

typedef enum {
  FALSE = 0,
  TRUE = 1
} BOOL;

typedef struct {
  char lexema[MAX_LONG_ID+1];
  int tipo;
  int valor_entero;
  BOOL valor_boolean;
  int es_direccion;
  int etiqueta;
} tipo_atributos;

/* CATEGORIA */
#define VARIABLE 1
#define PARAMETRO 2
#define FUNCION 3

/* CLASE */
#define ESCALAR 1
#define VECTOR 2

/* TIPO */
#define BOOLEAN 1
#define INT 2


#define TAMANO_HASH 757

#endif
