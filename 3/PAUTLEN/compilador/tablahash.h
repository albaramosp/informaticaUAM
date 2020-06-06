/*
 * Librería que define las estructuras y funciones necesarias para manejar
 * las tablas hash local y global necesarias para implementar la tabla de
 * simbolos. La estructura se ha cambiado respecto a la practica anterior
 * debido a que era bastante compleja, faltaban funciones que ahora son
 * necesarias y no permitia actualizar elementos de una funcion, o insertar
 * decidiendo si se hace en la tabla local o global.
 *
 * Alba Ramos Pedroviejo
 * Andrea Salcedo Lopez
 */

#ifndef TABLAHASH_H
#define	TABLAHASH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "alfa.h"
typedef struct {
  char lexema[MAX_LONG_ID+1];
  int categoria;
  int clase;
  int tipo;
  int tamano;
  int valor_entero;
  int numero_variables_locales;
  int posicion_variable_local;
  int numero_parametros;
  int posicion_parametros;
} nodo;

typedef nodo **TablaHash;

TablaHash crearTablaHash();
void destruirTablaHash(TablaHash t);
int hash(char *lexema);

int getPosicion (TablaHash t, char *x);
int busqueda(TablaHash t, char *lexema);

BOOL actualizarParamFuncion(TablaHash t, char *lexema, int numero_param);

BOOL insercion(TablaHash t, char *lexema, int categoria, int clase, int tipo, int tamano, int numero_variables_locales, int posicion_variable_local, int numero_parametros, int posicion_parametros);

int getCategoria(TablaHash t, char *lexema);
int getTipo(TablaHash t, char *lexema);
int getClase(TablaHash t, char *lexema);
int getTamano(TablaHash t, char *lexema);
int getNParams(TablaHash t, char *lexema);
int getPosParam(TablaHash t, char *lexema);
int getPosVar(TablaHash t, char *lexema);

#ifdef	__cplusplus
}
#endif

#endif
