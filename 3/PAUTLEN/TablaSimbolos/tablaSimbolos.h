/*
 * Librería que define las estructuras y funciones necesarias para manejar
 * la tabla de símbolos del compilador. Esta se basa en dos tablas hash
 * definidas según la librería hash.h: una tabla para símbolos globales y
 * otra para los locales. Permite realizar inserciones, búsquedas y aperturas
 * y cierres de ámbitos.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"

#define MAX_TABLA 10
#define DEFAULT_PARAMS 1
#define DEFAULT_LOCALS 1

typedef struct {
  TABLA_HASH *tablaGlobal;
  TABLA_HASH *tablaLocal;
} TABLA_SIMBOLOS;

TABLA_SIMBOLOS *crearTablaSimbolos();
Status aperturaAmbitoLocal(TABLA_SIMBOLOS *t, const char *id, int tipo, int adic1, int adic2);
Status cierreAmbitoLocal(TABLA_SIMBOLOS *t);
void borrarTablaSimbolos(TABLA_SIMBOLOS *t);
Status insercion(TABLA_SIMBOLOS *t, char *lexema, Categoria categoria, Clase clase, Tipo tipo, int tam, int adicional1, int adicional2);
int busqueda(TABLA_SIMBOLOS *t, char *id);
